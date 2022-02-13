#include "socket_server.h"

#define SOCKET_TYPE_INVALID         0
#define SOCKET_TYPE_RESERVE         1
#define SOCKET_TYPE_PLISTEN         2
#define SOCKET_TYPE_LISTEN          3
#define SOCKET_TYPE_CONNECTING      4
#define SOCKET_TYPE_CONNECTED       5
#define SOCKET_TYPE_HALFCLOSE_READ  6
#define SOCKET_TYPE_HALFCLOSE_WRITE 7
#define SOCKET_TYPE_PACCEPT         8
#define SOCKET_TYPE_BIND            9

#define PROTOCOL_TCP        0
#define PROTOCOL_UDP        1
#define PROTOCOL_UDPv6      2
#define PROTOCOL_UNKNOWN   255

#define PRIORITY_HIGH 0
#define PRIORITY_LOW  1

// MAX_SOCKET will be 2^MAX_SOCKET_P
#define MAX_SOCKET_P 16
#define MAX_SOCKET   (1 << MAX_SOCKET_P)

#define HASH_ID(id)  (((unsigned)(id)) % MAX_SOCKET)
#define ID_TAG16(id) ((((unsigned)(id)) >> MAX_SOCKET_P) & 0xFFFF)

#define MAX_INFO 128
#define MIN_READ_BUFFER 64
#define MAX_UDP_PACKAGE 65535

// ipv6 128bit + port 16bit + 1 byte type
#define UDP_ADDRESS_SIZE 19

#define WARNING_SIZE (1024*1024)

struct write_buffer {
    struct write_buffer * next;
    const void * buffer;
    char * ptr;
    size_t sz;
    bool userobject;
};

struct write_buffer_udp {
    struct write_buffer buffer;
    uint8_t udp_address[UDP_ADDRESS_SIZE];
};

struct wb_list {
    struct write_buffer * head;
    struct write_buffer * tail;
};

struct socket_stat {
    uint64_t rtime;
    uint64_t wtime;
    uint64_t read;
    uint64_t write;
};

struct socket {
    uintptr_t opaque;
    struct wb_list high;
    struct wb_list low;
    int64_t wb_size;
    struct socket_stat stat;
    atomic_ulong sending;
    socket_t fd;
    int id;
    atomic_int type;
    uint8_t protocol;
    bool reading;
    bool writing;
    bool closing;
    atomic_int udpconnecting;
    int64_t warn_size;
    union {
        int size;
        uint8_t udp_address[UDP_ADDRESS_SIZE];
    } p;
    atomic_flag dw_lock;
    int dw_offset;
    const void * dw_buffer;
    size_t dw_size;
};

struct socket_server {
    volatile uint64_t time;
    pipe_t ctrl_fd;
    int checkctrl;
    spoll_t event_fd;
    atomic_int alloc_id;
    int event_n;
    int event_index;
    struct socket_object_interface soi;
    spoll_event_t ev;
    struct socket slot[MAX_SOCKET];
    char buffer[MAX_INFO];
    uint8_t udpbuffer[MAX_UDP_PACKAGE];
    fd_set rfds;
};

struct request_open {
    int id;
    int port;
    uintptr_t opaque;
    char host[1];
};

struct request_send {
    int id;
    size_t sz;
    const void * buffer;
};

struct request_send_udp {
    struct request_send send;
    uint8_t address[UDP_ADDRESS_SIZE];
};

struct request_setudp {
    int id;
    uint8_t address[UDP_ADDRESS_SIZE];
};

struct request_close {
    int id;
    int shutdown;
    uintptr_t opaque;
};

struct request_listen {
    int id;
    int fd;
    uintptr_t opaque;
    char host[1];
};

struct request_bind {
    int id;
    socket_t fd;
    uintptr_t opaque;
};

struct request_resumepause {
    int id;
    uintptr_t opaque;
};

struct request_setopt {
    int id;
    int what;
    int value;
};

struct request_udp {
    int id;
    socket_t fd;
    int family;
    uintptr_t opaque;
};

/*
    The first byte is TYPE

    S Start socket
    B Bind socket
    L Listen socket
    K Close socket
    O Connect to (Open)
    X Exit
    D Send package (high)
    P Send package (low)
    A Send UDP package
    T Set opt
    U Create UDP socket
    C Set UDP address
    Q Query info
 */

struct request_package {
    uint8_t header[8]; // 6 bytes dummy
    union {
        char buffer[256];
        struct request_open open;
        struct request_send send;
        struct request_send_udp send_udp;
        struct request_close close;
        struct request_listen listen;
        struct request_resumepause resumepause;
        struct request_setopt setopt;
        struct request_udp udp;
        struct request_setudp set_udp;
    } u;
    uint8_t dummy[256];
};

union sockaddr_all {
    struct sockaddr s;
    struct sockaddr_in s4;
    struct sockaddr_in6 s6;
};

struct send_object {
    const void * buffer;
    size_t sz;
    void (* free_func)(void * buffer);
};

struct socket_lock {
    atomic_flag * lock;
    volatile int count;
};

static inline void socket_lock_init(struct socket_lock * sl, struct socket * s) {
    sl->lock = &s->dw_lock;
    sl->count = 0;
}

static inline void socket_lock(struct socket_lock * sl) {
    if (sl->count == 0) {
        atomic_flag_lock(sl->lock);
    }
    ++sl->count;
}

static inline int socket_trylock(struct socket_lock * sl) {
    if (sl->count == 0) {
        if (!atomic_flag_trylock(sl->lock)) {
            // lock failed
            return 0;
        }
    }
    ++sl->count;
    return 1;
}

static inline void socket_unlock(struct socket_lock * sl) {
    --sl->count;
    if (sl->count <= 0) {
        assert(sl->count == 0);
        atomic_flag_unlock(sl->lock);
    }
}

static inline int socket_invalid(struct socket * s, int id) {
    return s->id != id || atomic_load(&s->type) == SOCKET_TYPE_INVALID;
}

inline void send_object_init_memory(struct send_object * so, const void * object, size_t sz) {
    so->buffer = object;
    so->sz = sz;
    so->free_func = free;
}

inline void send_object_init_object(struct socket_server * ss, struct send_object * so, const void * object) {
    so->buffer = ss->soi.buffer(object);
    so->sz = ss->soi.size(object);
    so->free_func = ss->soi.free;
}
 
static inline bool send_object_init(struct socket_server * ss, struct send_object * so, const void * object, size_t sz) {
    if (sz == SOCKET_SENDBUFFER_USEROBJECT) {
        send_object_init_object(ss, so, object);
        return true;
    }

    send_object_init_memory(so, object, sz);
    return false;
}

static inline void dummy_free(void * ptr) {
    (void)ptr;
}

static inline void send_object_init_from_sendbuffer(struct socket_server * ss, struct send_object * so, struct socket_sendbuffer * buf) {
    switch (buf->type) {
    case SOCKET_BUFFER_MEMORY:
        send_object_init_memory(so, buf->buffer, buf->sz);
        break;
    case SOCKET_BUFFER_OBJECT:
        send_object_init_object(ss, so, buf->buffer);
        break;
    case SOCKET_BUFFER_RAWPOINTER:
        so->buffer = buf->buffer;
        so->sz = buf->sz;
        so->free_func = dummy_free;
        break;
    default:
        // never get here
        PERR("type=%d", buf->type);
        so->buffer = NULL;
        so->sz = 0;
        so->free_func = NULL;
        break;
    }
}

static inline void write_buffer_free(struct socket_server * ss, struct write_buffer * wb) {
    if (wb->userobject) {
        ss->soi.free((void *)wb->buffer);
    } else {
        free((void *)wb->buffer);
    }
    free(wb);
}

static int reserve_id(struct socket_server * ss) {
    for (int i = 0; i < MAX_SOCKET; ++i) {
        int id = atomic_fetch_add(&ss->alloc_id, 1) + 1;
        if (id < 0) {
            id = atomic_fetch_and(&ss->alloc_id, 0x7FFFFFFF) & 0x7FFFFFFF;
        }

        struct socket * s = &ss->slot[HASH_ID(id)];
        int type_invalid = atomic_load(&s->type);
        if (type_invalid == SOCKET_TYPE_INVALID) {
            if (atomic_compare_exchange_strong(&s->type, &type_invalid, SOCKET_TYPE_RESERVE)) {
                s->id = id;
                s->protocol = PROTOCOL_UNKNOWN;
                // socket_server_udp_connect may inc s->udpconnecting directly (from other thread, before new_fd),
                // so reset it to 0 here rather than in new_fd
                atomic_init(&s->udpconnecting, 0);
                s->fd = INVALID_SOCKET;
                return id;
            }

            // retry
            --i;
        }
    }

    return -1;
}

static inline void clear_wb_list(struct wb_list * list) {
    list->head = NULL;
    list->tail = NULL;
}

struct socket_server * 
socket_server_create(uint64_t time) {
    spoll_t efd = spoll_create();
    if (spoll_invalid(efd)) {
        RETNUL("socket-server: create event pool failed.");
    }

    pipe_t chfd;
    if (pipe_open(chfd)) {
        PERR("socket-server: create socket pair failed.");
        spoll_delete(efd);
        return NULL;
    }

    if (spoll_add(efd, chfd->recv, NULL)) {
        // add recvctrl_fd (ctrl_fd recv) to event poll
        PERR("socket-server: can't add server fd to event pool.");
        pipe_close(chfd);
        spoll_delete(efd);
        return NULL;
    }

    struct socket_server * ss = malloc(sizeof(struct socket_server));
    ss->time = time;
    ss->event_fd = efd;
    ss->ctrl_fd[0] = chfd[0];
    ss->checkctrl = 1;

    for (int i = 0; i < MAX_SOCKET; ++i) {
        struct socket * s = &ss->slot[i];
        atomic_init(&s->type, SOCKET_TYPE_INVALID);
        clear_wb_list(&s->high);
        clear_wb_list(&s->low);
        s->dw_lock = (atomic_flag)ATOMIC_FLAG_INIT;
    }

    atomic_init(&ss->alloc_id, 0);
    ss->event_n = 0;
    ss->event_index = 0;
    memset(&ss->soi, 0, sizeof(ss->soi));
    FD_ZERO(&ss->rfds);
    assert(ss->ctrl_fd->recv < FD_SETSIZE);

    return ss;
}

void
socket_server_updatetime(struct socket_server * ss, uint64_t time) {
    ss->time = time;
}

static void free_wb_list(struct socket_server * ss, struct wb_list * list) {
    struct write_buffer * wb = list->head;
    while (wb) {
        struct write_buffer * tmp = wb;
        wb = wb->next;
        write_buffer_free(ss, tmp);
    }
    list->head = NULL;
    list->tail = NULL;
}

static void free_buffer(struct socket_server * ss, struct socket_sendbuffer * buf) {
    void * buffer = (void *)buf->buffer;
    switch (buf->type) {
    case SOCKET_BUFFER_MEMORY:
        free(buffer);
        break;
    case SOCKET_BUFFER_OBJECT:
        ss->soi.free(buffer);
        break;
    case SOCKET_BUFFER_RAWPOINTER:
        break;
    }
}

static void force_close(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    result->id = s->id;
    result->ud = 0;
    result->data = NULL;
    result->opaque = s->opaque;

    uint8_t type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_INVALID) {
        return;
    }
    assert(type != SOCKET_TYPE_RESERVE);

    free_wb_list(ss, &s->high);
    free_wb_list(ss, &s->low);
    spoll_del(ss->event_fd, s->fd);

    socket_lock(sl);
    if (type != SOCKET_TYPE_BIND) {
        if (closesocket(s->fd)) {
            PERR("close socket:");
        }
    }
    atomic_store(&s->type, SOCKET_TYPE_INVALID);
    if (s->dw_buffer) {
        // see free_buffer
        void * buffer = (void *)s->dw_buffer;
        if (s->dw_size == SOCKET_SENDBUFFER_USEROBJECT) {
            ss->soi.free(buffer);
        } else {
            free(buffer);
        }
        s->dw_buffer = NULL;
    }
    socket_unlock(sl);
}

void socket_server_release(struct socket_server * ss) {
    struct socket_message dummy;
    for (int i = 0; i < MAX_SOCKET; ++i) {
        struct socket * s = ss->slot + i;
        struct socket_lock sl; socket_lock_init(&sl, s);
        if (atomic_load(&s->type) != SOCKET_TYPE_RESERVE) {
            force_close(ss, s, &sl, &dummy);
        }
    }
    pipe_close(ss->ctrl_fd);
    spoll_delete(ss->event_fd);
    free(ss);
}

static inline void check_wb_list(struct wb_list * list) {
    assert(list->head == NULL);
    assert(list->tail == NULL);
}

static inline int enable_write(struct socket_server * ss, struct socket * s, bool enable) {
    if (s->writing != enable) {
        s->writing = enable;
        return spoll_mod(ss->event_fd, s->fd, s, s->reading, enable);
    }
    return 0;
}

static inline int enable_read(struct socket_server * ss, struct socket * s, bool enable) {
    if (s->reading != enable) {
        s->reading = enable;
        return spoll_mod(ss->event_fd, s->fd, s, enable, s->writing);
    }
    return 0;
}

static struct socket * new_fd(struct socket_server * ss, int id, socket_t fd, int protocol, uintptr_t opaque, bool reading) {
    struct socket * s = ss->slot + HASH_ID(id);
    assert(atomic_load(&s->type) == SOCKET_TYPE_RESERVE);

    if (spoll_add(ss->event_fd, fd, s)) {
        atomic_store(&s->type, SOCKET_TYPE_INVALID);
        RETNUL("new fd spoll add error");
    }

    s->id = id;
    s->fd = fd;
    s->reading = true;
    s->writing = false;
    s->closing = false;
    atomic_init(&s->sending, ID_TAG16(id) << 16);
    s->protocol = protocol;
    s->p.size = MIN_READ_BUFFER;
    s->opaque = opaque;
    s->wb_size = 0;
    s->warn_size = 0;
    check_wb_list(&s->high);
    check_wb_list(&s->low);
    s->dw_buffer = NULL;
    s->dw_size = 0;
    memset(&s->stat, 0, sizeof(struct socket_stat));
    if (enable_read(ss, s, reading)) {
        atomic_store(&s->type, SOCKET_TYPE_INVALID);
        RETNUL("enable read error reading=%d", reading);
    }
    return s;
}

static inline void stat_read(struct socket_server * ss, struct socket * s, int n) {
    s->stat.read += n;
    s->stat.rtime = ss->time;
}

static inline void stat_write(struct socket_server * ss, struct socket * s, int n) {
    s->stat.write += n;
    s->stat.wtime = ss->time;
}

// return -1 when connecting
static int open_socket(struct socket_server * ss, struct request_open * request, struct socket_message * result) {
    int id = request->id;

    result->opaque = request->opaque;
    result->id = id;
    result->ud = 0;
    result->data = NULL;

    char port[16];
    sprintf(port, "%d", request->port);

    struct addrinfo * ai_list = NULL;
    struct addrinfo ai_hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
    };
    int status = getaddrinfo(request->host, port, &ai_hints, &ai_list);
    if (status != 0) {
        result->data = (char *)gai_strerror(status);
        goto _getaddrinfo_failed;
    }

    socket_t sock = INVALID_SOCKET;
    struct addrinfo * ai_ptr = ai_list;
    for (; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next) {
        sock = socket(ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol);
        if (sock == INVALID_SOCKET) {
            continue;
        }

        socket_set_keepalive(sock);
        socket_set_nonblock(sock);
        status = connect(sock, ai_ptr->ai_addr, ai_ptr->ai_addrlen);
        if (status != 0 && errno != EINPROGRESS) {
            close(sock);
            sock = INVALID_SOCKET;
            continue;
        }

        break;
    }

    if (sock == INVALID_SOCKET) {
        result->data = strerror(errno);
        goto _failed;
    }

    struct socket * ns = new_fd(ss, id, sock, PROTOCOL_TCP, request->opaque, true);
    if (ns == NULL) {
        result->data = "reach skynet socket number limit";
        closesocket(sock);
        goto _failed;
    }

    if (status == 0) {
        atomic_store(&ns->type, SOCKET_TYPE_CONNECTED);
        struct sockaddr * addr = ai_ptr->ai_addr;
        void * sin_addr = ai_ptr->ai_family == AF_INET ? 
            (void *)&((struct sockaddr_in *)addr)->sin_addr : 
                (void *)&((struct sockaddr_in6 *)addr)->sin6_addr;
        if (inet_ntop(ai_ptr->ai_family, sin_addr, ss->buffer, sizeof(ss->buffer))) {
            result->data = ss->buffer;
        }
        freeaddrinfo(ai_list);
        return SOCKET_OPEN;
    }
    // EINPROGRESS -> SOCKET_TYPE_CONNECTING
    atomic_store(&ns->type, SOCKET_TYPE_CONNECTING);
    if (enable_write(ss, ns, true)) {
        result->data = "enable write failed";
        goto _failed;
    }
    freeaddrinfo(ai_list);
    return -1;

_failed:
    freeaddrinfo(ai_list);
_getaddrinfo_failed:
	atomic_store(&ss->slot[HASH_ID(id)].type, SOCKET_TYPE_INVALID);
    return SOCKET_ERR;
}

static inline int report_error(struct socket * s, struct socket_message * result, const char * err) {
    result->id = s->id;
    result->ud = 0;
    result->opaque = s->opaque;
    result->data = (char *)err;
    return SOCKET_ERR;
}

static int close_write(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    if (s->closing) {
        force_close(ss, s, sl, result);
        return SOCKET_RST;
    }

    int type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_HALFCLOSE_READ) {
        // recv 0 before, ignore the error and close fd
        force_close(ss, s, sl, result);
        return SOCKET_RST;
    }
    if (type == SOCKET_TYPE_HALFCLOSE_WRITE) {
        // already raise SOCKET_ERR
        return SOCKET_RST;
    }
    atomic_store(&s->type, SOCKET_TYPE_HALFCLOSE_WRITE);
    shutdown(s->fd, SHUT_WR);
    enable_write(ss, s, false);
    return report_error(s, result, strerror(errno));
}

static int send_list_tcp(struct socket_server * ss, struct socket * s, struct wb_list * list, struct socket_lock * sl, struct socket_message * result) {
    while (list->head) {
        struct write_buffer * tmp = list->head;
        for (;;) {
            int sz = socket_send(s->fd, tmp->ptr, tmp->sz);
            if (sz < 0) {
                switch (errno) {
                case EINTR:
                    continue;
                case EAGAIN:
                    return -1;
                }
                return close_write(ss, s, sl, result);
            }
            stat_write(ss, s, sz);
            s->wb_size -= sz;
            if ((size_t)sz != tmp->sz) {
                tmp->ptr += sz;
                tmp->sz -= sz;
                return -1;
            }
            break;
        }
        list->head = tmp->next;
        write_buffer_free(ss, tmp);
    }
    list->tail = NULL;

    return -1;
}

static socklen_t udp_socket_address(struct socket * s, const uint8_t udp_address[UDP_ADDRESS_SIZE], union sockaddr_all * sa) {
    int type = (uint8_t)udp_address[0];
    if (type != s->protocol)
        return 0;
    
    uint16_t port = 0;
    memcpy(&port, udp_address+1, sizeof(uint16_t));
    switch (s->protocol) {
    case PROTOCOL_UDP:
        memset(&sa->s4, 0, sizeof(sa->s4));
        sa->s.sa_family = AF_INET;
        sa->s4.sin_port = port;
        // ipv4 address is 32 bits
        memcpy(&sa->s4.sin_addr, udp_address+1+sizeof(uint16_t), sizeof(sa->s4.sin_addr));
        return sizeof(sa->s4);
    case PROTOCOL_UDPv6:
        memset(&sa->s6, 0, sizeof(sa->s6));
        sa->s.sa_family = AF_INET6;
        sa->s6.sin6_port = port;
        // ipv6 address is 128 bits
        memcpy(&sa->s6.sin6_addr, udp_address+1+sizeof(uint16_t), sizeof(sa->s6.sin6_addr));
        return sizeof(sa->s6);
    }
    return 0;
}

static void drop_udp(struct socket_server * ss, struct socket * s, struct wb_list * list, struct write_buffer * tmp) {
    s->wb_size -= tmp->sz;
    list->head = tmp->next;
    if (list->head == NULL)
        list->tail = NULL;
    write_buffer_free(ss, tmp);
}

static int send_list_udp(struct socket_server * ss, struct socket * s, struct wb_list * list) {
    while (list->head) {
        union sockaddr_all sa;
        struct write_buffer * tmp = list->head;
        struct write_buffer_udp * udp = (struct write_buffer_udp *)tmp;
        socklen_t sasz = udp_socket_address(s, udp->udp_address, &sa);
        if (sasz == 0) {
            PERR("socket-server : udp (%d) type mismatch.", s->id);
            drop_udp(ss, s, list, tmp);
            return -1;
        }
        int err = socket_sendto(s->fd, tmp->ptr, tmp->sz, &sa.s, sasz);
        if (err < 0) {
            switch (errno) {
            case EINTR:
            case EAGAIN:
                return -1;
            }
            PERR("socket-server : udp (%d) sendto error.", s->id);
            drop_udp(ss, s, list, tmp);
            return -1;
        }
        stat_write(ss, s, tmp->sz);
        s->wb_size -= tmp->sz;
        list->head = tmp->next;
        write_buffer_free(ss, tmp);
    }
    list->tail = NULL;

    return -1;
}

static int send_list(struct socket_server * ss, struct socket * s, struct wb_list * list, struct socket_lock * sl, struct socket_message * result) {
    if (s->protocol == PROTOCOL_TCP)
        return send_list_tcp(ss, s, list, sl, result);
    return send_list_udp(ss, s, list);
}

static inline int list_uncomplete(struct wb_list * list) {
    struct write_buffer * wb = list->head;
    if (wb == NULL)
        return 0;

    return (const void *)wb->ptr != wb->buffer;
}

static void raise_uncomplete(struct socket * s) {
    struct wb_list * low = &s->low;
    struct write_buffer * tmp = low->head;
    low->head = tmp->next;
    if (low->head == NULL) {
        low->tail = NULL;
    }

    // move head of low list (tmp) to the empty high list
    struct wb_list * high = &s->high;
    assert(high->head == NULL);

    tmp->next = NULL;
    high->head = high->tail = tmp;
}

static inline int send_buffer_empty(struct socket * s) {
    return s->high.head == NULL && s->low.head == NULL;
}

/*
    Each socket has two write buffer list, high priority and low priority.

    1. send high list as far as possiable.
    2. If high list is empty, try to send low list.
    3. If low list head is uncomplete (send a part befor), move the head of low list to empty high list (call raise_uncomplete).
    4. If two lists are both empty, turn off the event. (call check_close)
 */
static int send_buffer_partial(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    assert(!list_uncomplete(&s->low));
    // step 1
    int ret = send_list(ss, s, &s->high, sl, result);
    if (ret != -1) {
        if (ret == SOCKET_ERR) {
            // HALFCLOSE_WRITE
            return SOCKET_ERR;
        }
        // SOCKET_RST (ignore)
        return -1;
    }
    if (s->high.head == NULL) {
        // step 2
        if (s->low.head != NULL) {
            int ret = send_list(ss, s, &s->low, sl, result);
            if (ret != -1) {
                if (ret == SOCKET_ERR) {
                    // HALFCLOSE_WRITE
                    return SOCKET_ERR;
                }
                // SOCKET_RST (ignore)
                return -1;
            }
            // step 3
            if (list_uncomplete(&s->low)) {
                raise_uncomplete(s);
                return -1;
            }
            if (s->low.head)
                return -1;
        }
        // step 4
        assert(send_buffer_empty(s) && s->wb_size == 0);

        if (s->closing) {
            // finish writing
            force_close(ss, s, sl, result);
            return -1;
        }

        int err = enable_write(ss, s, false);
        if (err) {
            return report_error(s, result, "disable write failed");
        }

        if (s->warn_size > 0) {
            s->warn_size = 0;
            result->opaque = s->opaque;
            result->id = s->id;
            result->ud = 0;
            result->data = NULL;
            return SOCKET_WARNING;
        }
    }

    return -1;
}

static int send_buffer(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    if (!socket_trylock(sl))
        return -1; // blocked by direct write, send later.
    
    if (s->dw_buffer) {
        // add direct write buffer before high.head
        struct write_buffer * buf = malloc(sizeof(struct write_buffer));
        struct send_object so;
        buf->userobject = send_object_init(ss, &so, (void *)s->dw_buffer, s->dw_size);
        buf->ptr = (char *)so.buffer + s->dw_offset;
        buf->sz = so.sz - s->dw_offset;
        buf->buffer = (void *)s->dw_buffer;
        s->wb_size += buf->sz;
        if (s->high.head == NULL) {
            s->high.head = s->high.tail = buf;
            buf->next = NULL;
        } else {
            buf->next = s->high.head;
            s->high.head = buf;
        }
        s->dw_buffer = NULL;
    }

    int r = send_buffer_partial(ss, s, sl, result);
    socket_unlock(sl);

    return r;
}

static struct write_buffer * append_sendbuffer_partial(struct socket_server * ss, struct wb_list * s, struct request_send * request, int size) {
    struct write_buffer * buf = malloc(size);
    struct send_object so;
    buf->userobject = send_object_init(ss, &so, request->buffer, request->sz);
    buf->ptr = (char *)so.buffer;
    buf->sz = so.sz;
    buf->buffer = request->buffer;
    buf->next = NULL;
    if (s->head == NULL) {
        s->head = s->tail = buf;
    } else {
        assert(s->tail != NULL && s->tail->next == NULL);
        s->tail->next = buf;
        s->tail = buf;
    }
    return buf;
}

static inline void append_sendbuffer_udp(struct socket_server * ss, struct socket * s, int priority, struct request_send * request, const uint8_t udp_address[UDP_ADDRESS_SIZE]) {
    struct wb_list * list = priority == PRIORITY_HIGH ? &s->high : &s->low;
    struct write_buffer_udp * buf = (struct write_buffer_udp *)append_sendbuffer_partial(ss, list, request, sizeof(struct write_buffer_udp));
    memcpy(buf->udp_address, udp_address, UDP_ADDRESS_SIZE);
    s->wb_size += buf->buffer.sz;
}

static inline void append_sendbuffer(struct socket_server * ss, struct socket * s, struct request_send * request) {
    struct write_buffer * buf = append_sendbuffer_partial(ss, &s->high, request, sizeof(struct write_buffer));
    s->wb_size += buf->sz;
}

static inline void append_sendbuffer_low(struct socket_server * ss, struct socket * s, struct request_send * request) {
    struct write_buffer * buf = append_sendbuffer_partial(ss, &s->low, request, sizeof(struct write_buffer));
    s->wb_size += buf->sz;
}

static int trigger_write(struct socket_server * ss, struct request_send * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = &ss->slot[HASH_ID(id)];
    if (socket_invalid(s, id))
        return -1;
    if (enable_write(ss, s, true)) {
        return report_error(s, result, "enable write failed");
    }
    return -1;
}

/*
    When send a package, we can assign the priority : PRIORITY_HIGH or PRIORITY_LOW

    If socket buffer is empty, write to fd directly.
        If write a part, append the rest part to high list. (Even priority is PRIORITY_LOW)
    Else append package to high (PRIORITY_HIGH) or low (PRIORITY_LOW) list
 */
static int send_socket(struct socket_server * ss, struct request_send * request, struct socket_message * result, int priority, const uint8_t * udp_address) {
    int id = request->id;
    struct socket * s = ss->slot + HASH_ID(id);
    struct send_object so;
    send_object_init(ss, &so, request->buffer, request->sz);
    uint8_t type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_INVALID || s->id != id
        || type == SOCKET_TYPE_HALFCLOSE_WRITE
        || type == SOCKET_TYPE_PACCEPT
        || s->closing) {
        so.free_func((void *)request->buffer);
    }
    if (type == SOCKET_TYPE_PLISTEN || type == SOCKET_TYPE_LISTEN) {
        PERR("socket-server: write to listen fd %d.", id);
        so.free_func((void *)request->buffer);
        return -1;
    }
    if (send_buffer_empty(s)) {
        if (s->protocol == PROTOCOL_TCP) {
            append_sendbuffer(ss, s, request); // add to high priority list, even priority == PRIORITY_LOW
        } else {
            // udp
            if (udp_address == NULL) {
                udp_address = s->p.udp_address;
            }
            union sockaddr_all sa;
            socklen_t sasz = udp_socket_address(s, udp_address, &sa);
            if (sasz == 0) {
                // udp type mismatch, just drop it.
                PERR("socket-server: udp socket (%d) type mismatch.", id);
                so.free_func((void *)request->buffer);
                return -1;
            }
            int n = socket_sendto(s->fd, so.buffer, so.sz, &sa.s, sasz);
            if ((size_t)n != so.sz) {
                append_sendbuffer_udp(ss, s, priority, request, udp_address);
            } else {
                stat_write(ss, s, n);
                so.free_func((void *)request->buffer);
                return -1;
            }
        }
        if (enable_write(ss, s, true)) {
            return report_error(s, result, "enable write failed");
        }
    } else {
        if (s->protocol == PROTOCOL_TCP) {
            if (priority == PRIORITY_LOW) {
                append_sendbuffer_low(ss, s, request);
            } else {
                append_sendbuffer(ss, s, request);
            }
        } else {
            if (udp_address == NULL) {
                udp_address = s->p.udp_address;
            }
            append_sendbuffer_udp(ss, s, priority, request, udp_address);
        }
    }
    if (s->wb_size >= WARNING_SIZE && s->wb_size >= s->warn_size) {
        s->warn_size = s->warn_size == 0 ? WARNING_SIZE * 2 : s->warn_size * 2;
        result->opaque = s->opaque;
        result->id = s->id;
        result->ud = s->wb_size % 1024 == 0 ? s->wb_size/1024 : s->wb_size/1024+1;
        result->data = NULL;
        return SOCKET_WARNING;
    }
    return -1;
}

static int listen_socket(struct socket_server * ss, struct request_listen * request, struct socket_message * result) {
    int id = request->id;
    socket_t listen_fd = request->fd;
    struct socket * s = new_fd(ss, id, listen_fd, PROTOCOL_TCP, request->opaque, false);
    if (s == NULL) {
        goto _failed;
    }
    atomic_store(&s->type, SOCKET_TYPE_PLISTEN);
    return -1;
_failed:
    close(listen_fd);
    result->opaque = request->opaque;
    result->id = id;
    result->ud = 0;
    result->data = "reach skynet socket number limit";
    atomic_store(&ss->slot[HASH_ID(id)].type, SOCKET_TYPE_INVALID);

    return SOCKET_ERR;
}

static inline int nomore_sending_data(struct socket * s) {
    return (send_buffer_empty(s) && s->dw_buffer == NULL && (atomic_load(&s->sending) & 0xFFFF) == 0)
        || (atomic_load(&s->type) == SOCKET_TYPE_HALFCLOSE_WRITE);
}

static void close_read(struct socket_server * ss, struct socket * s, struct socket_message * result) {
    // Don't read socket later
    atomic_store(&s->type, SOCKET_TYPE_HALFCLOSE_READ);
    enable_read(ss, s, false);
    shutdown(s->fd, SHUT_RD);
    result->id = s->id;
    result->ud = 0;
    result->data = NULL;
    result->opaque = s->opaque;
}

static inline int halfclose_read(struct socket * s) {
    return atomic_load(&s->type) == SOCKET_TYPE_HALFCLOSE_READ;
}

// SOCKET_CLOSE can be raised (only once) in one of two conditions.
// See https://github.com/cloudwu/skynet/issues/1346 for more discussion.
// 1. close socket by self, See close_socket()
// 2. recv 0 or eof event (close socket by remote), See forward_message_tcp()
// It's able to write data after SOCKET_CLOSE (In condition 2), but if remote is closed, SOCKET_ERR may raised.
static int close_socket(struct socket_server * ss, struct request_close * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = ss->slot + HASH_ID(id);
    if (socket_invalid(s, id)) {
        // The socket is closed, ignore
        return -1;
    }

    struct socket_lock sl; socket_lock_init(&sl, s);
    int shutdown_read = halfclose_read(s);
    if (request->shutdown || nomore_sending_data(s)) {
        // If socket is SOCKET_TYPE_HALFCLOSE_READ, Do not raise SOCKET_CLOSE again.
        int r = shutdown_read ? -1 : SOCKET_CLOSE;
        force_close(ss, s, &sl, result);
        return r;
    }
    s->closing = true;
    if (!shutdown_read) {
        // don't read socket after socket.close()
        close_read(ss, s, result);
        return SOCKET_CLOSE;
    }
    // recv 0 before (socket is SOCKET_TYPE_HALFCLOSE_READ) and waiting for sending data out.
    return -1;
}

static int bind_socket(struct socket_server * ss, struct request_bind * request, struct socket_message * result) {
    int id = request->id;
    result->id = id;
    result->opaque = request->opaque;
    result->ud = 0;
    struct socket * s = new_fd(ss, id, request->fd, PROTOCOL_TCP, request->opaque, true);
    if (s == NULL) {
        result->data = "reach skynet socket number limit";
        return SOCKET_ERR;
    }
    socket_set_nonblock(request->fd);
    atomic_store(&s->type, SOCKET_TYPE_BIND);
    result->data = "binding";
    return SOCKET_OPEN;
}

static int resume_socket(struct socket_server * ss, struct request_resumepause * request, struct socket_message * result) {
    int id = request->id;
    result->id = id;
    result->opaque = request->opaque;
    result->ud = 0;
    result->data = NULL;

    struct socket * s = ss->slot + HASH_ID(id);
    if (socket_invalid(s, id)) {
        result->data = "invalid socket";
        return SOCKET_ERR;
    }
    if (halfclose_read(s)) {
        // The closing socket may be in transit, so raise an error. See https://github.com/cloudwu/skynet/issues/1374
        result->data = "socket closed";
        return SOCKET_ERR;
    }
    
    struct socket_lock sl; socket_lock_init(&sl, s);
    if (enable_read(ss, s, true)) {
        result->data = "enable read failed";
        return SOCKET_ERR;
    }

    uint8_t type = atomic_load(&s->type);
    if (type == SOCKET_TYPE_PACCEPT || type == SOCKET_TYPE_PLISTEN) {
        atomic_store(&s->type, type == SOCKET_TYPE_PACCEPT ? SOCKET_TYPE_CONNECTED : SOCKET_TYPE_LISTEN);
        s->opaque = request->opaque;
        result->data = "start";
        return SOCKET_OPEN;
    } else if (type == SOCKET_TYPE_CONNECTED) {
        // todo: maybe we should send a message SOCKET_TRANSFER to s->opaque
        s->opaque = request->opaque;
        result->data = "transfer";
        return SOCKET_OPEN;
    }
    // if s->type == SOCKET_TYPE_HALFCLOSE_WRITE, SOCKET_CLOSE message will send later
    return -1;
}

static int pause_socket(struct socket_server * ss, struct request_resumepause * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = ss->slot + HASH_ID(id);
    if (socket_invalid(s, id)) {
        return -1;
    }
    if (enable_read(ss, s, false)) {
        return report_error(s, result, "enable read failed");
    }
    return -1;
}

static void setopt_socket(struct socket_server * ss, struct request_setopt * request) {
    int id = request->id;
    struct socket * s = ss->slot + HASH_ID(id);
    if (socket_invalid(s, id)) {
        return;
    }
    int v = request->value;
    setsockopt(s->fd, IPPROTO_TCP, request->what, &v, sizeof(v));
}

static void block_readpipe(pipe_t pipefd, void * buffer, int sz) {
    for (;;) {
        int n = pipe_recv(pipefd, buffer, sz);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            PERR("socket-server : read pipe error.");
            return;
        }
        // must atomic read from a pipe
        assert(n == sz);
        return;
    }
}

static inline int has_cmd(struct socket_server * ss) {
    struct timeval tv = {0,0};
    FD_SET(ss->ctrl_fd->recv, &ss->rfds);
    return select(ss->ctrl_fd->recv+1, &ss->rfds, NULL, NULL, &tv);
}

static void add_udp_socket(struct socket_server * ss, struct request_udp * request) {
    int id = request->id;
    int protocol = request->family == AF_INET6 ? PROTOCOL_UDPv6 : PROTOCOL_UDP;
    struct socket * ns = new_fd(ss, id, request->fd, protocol, request->opaque, true);
    if (ns == NULL) {
        closesocket(request->fd);
        atomic_store(&ss->slot[HASH_ID(id)].type, SOCKET_TYPE_INVALID);
        return;
    }
    atomic_store(&ns->type, SOCKET_TYPE_CONNECTED);
    memset(ns->p.udp_address, 0, sizeof(ns->p.udp_address));
}

static int set_udp_address(struct socket_server * ss, struct request_setudp * request, struct socket_message * result) {
    int id = request->id;
    struct socket * s = ss->slot + HASH_ID(id);
    if (socket_invalid(s, id)) {
        return -1;
    }
    int type = request->address[0];
    if (type != s->protocol) {
        // protocol mismatch
        return report_error(s, result, "protocol mismatch");
    }
    if (type == PROTOCOL_UDP) {
        memcpy(s->p.udp_address, request->address, 1+2+ 4); // 1 type, 2 port,  4 ipv4
    } else {
        memcpy(s->p.udp_address, request->address, 1+2+16); // 1 type, 2 port, 16 ipv6
    }
    atomic_fetch_sub(&s->udpconnecting, 1);
    return -1;
}

static inline void inc_sending_ref(struct socket * s, int id) {
    if (s->protocol != PROTOCOL_TCP)
        return;
    
    for (;;) {
        unsigned long sending = atomic_load(&s->sending);
        if ((sending >> 16) == ID_TAG16(id)) {
            if ((sending & 0xFFFF) == 0xFFFF) {
                // s->sending may overflow (rarely), so busy waiting here for socket thread dec it. see isuse #794
                continue;
            }
            // inc sending only matching the same socket id
            if (atomic_compare_exchange_strong(&s->sending, &sending, sending + 1)) 
                return;
            // atom inc failed, retry
        } else {
            // socket id changed, just return
            return;
        }
    }
}

static inline void dec_sending_ref(struct socket_server * ss, int id) {
    struct socket * s = ss->slot + HASH_ID(id);
    if (s->id == id && s->protocol == PROTOCOL_TCP) {
        assert((atomic_load(&s->sending) & 0xFFFF) != 0);
        atomic_fetch_sub(&s->sending, 1);
    }
}

// return type
static int ctrl_cmd(struct socket_server * ss, struct socket_message * result) {
    // the length of message is one byte, so 256+8 buffer size is enough
    uint8_t buffer[256];
    uint8_t header[2];
    block_readpipe(ss->ctrl_fd, header, sizeof(header));
    int type = header[0];
    int len = header[1];
    block_readpipe(ss->ctrl_fd, buffer, len);

    // ctrl command only exist in local fd, so don't worry about endian.
    switch (type) {
    case 'R':
        return resume_socket(ss, (struct request_resumepause *)buffer, result);
    case 'S':
        return pause_socket(ss, (struct request_resumepause *)buffer, result);
    case 'B':
        return bind_socket(ss, (struct request_bind *)buffer, result);
    case 'L':
        return listen_socket(ss, (struct request_listen *)buffer, result);
    case 'K':
        return close_socket(ss, (struct request_close *)buffer, result);
    case 'O':
        return open_socket(ss, (struct request_open *)buffer, result);
    case 'X':
        result->opaque = 0;
        result->id = 0;
        result->ud = 0;
        result->data = NULL;
        return SOCKET_EXIT;
    case 'W':
        return trigger_write(ss, (struct request_send *)buffer, result);
    case 'D':
    case 'P': {
        int priority = type == 'D' ? PRIORITY_HIGH : PRIORITY_LOW;
        struct request_send * request = (struct request_send *)buffer;
        int ret = send_socket(ss, request, result, priority, NULL);
        dec_sending_ref(ss, request->id);
        return ret;
    }
    case 'A': {
        struct request_send_udp * rsu = (struct request_send_udp *)buffer;
        return send_socket(ss, &rsu->send, result, PRIORITY_HIGH, rsu->address);
    }
    case 'C':
        return set_udp_address(ss, (struct request_setudp *)buffer, result);
    case 'T':
        setopt_socket(ss, (struct request_setopt *)buffer);
        return -1;
    case 'U':
        add_udp_socket(ss, (struct request_udp *)buffer);
        return -1;
    default:
        PERR("socket-server: Unknown ctrl %c, %d.", type, type);
        return -1;
    }
}

// return -1 (ignore) when error
static int forward_message_tcp(struct socket_server * ss, struct socket * s, struct socket_lock * sl, struct socket_message * result) {
    int sz = s->p.size;
    char * buffer = malloc(sz);
    int n = socket_recv(s->fd, buffer, sz);
    if (n < 0) {
        free(buffer);
        switch (errno) {
        case EINTR:
        case EAGAIN:
            break;
        default:
            return report_error(s, result, strerror(errno));
        }
        return -1;
    }
    if (n == 0) {
        free(buffer);
        if (s->closing) {
            // Rare case: if s->closing is true, reading event is disable, and SOCKET_CLOSE is raised.
            if (nomore_sending_data(s)) {
                force_close(ss, s, sl, result);
            }
            return -1;
        }
        int type = atomic_load(&s->type);
        if (type == SOCKET_TYPE_HALFCLOSE_READ) {
            // Rare case: Already shutdown read.
            return -1;
        }
        if (type == SOCKET_TYPE_HALFCLOSE_WRITE) {
            // Remote shutdown read (write error) before.
            force_close(ss, s, sl, result);
        } else {
            close_read(ss, s, result);
        }
        return SOCKET_CLOSE;
    }

    if (halfclose_read(s)) {
        // discard recv data (Rare case : if socket is HALFCLOSE_READ, reading event is disable.)
        free(buffer);
        return -1;
    }

    stat_read(ss, s, n);

    result->opaque = s->opaque;
    result->id = s->id;
    result->ud = n;
    result->data = buffer;

    if (n == sz) {
        s->p.size *= 2;
        return SOCKET_MORE;
    } else if (sz > MIN_READ_BUFFER && n*2 < sz) {
        s->p.size /= 2;
    }

    return SOCKET_DATA;
}
