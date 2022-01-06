#include "socket.h"

int 
socket_ntop(const sockaddr_t a, char ip[INET6_ADDRSTRLEN]) {
    const char * res;

    if (a->s.sa_family == AF_INET || a->len == sizeof(a->s4)) {
        res = inet_ntop(AF_INET, &a->s4.sin_addr, ip, INET_ADDRSTRLEN);
        if (res != NULL) {
            return ntohs(a->s4.sin_port); 
        }
        return -1;
    }

    if (a->s.sa_family == AF_INET6 || a->len == sizeof(a->s6)) {
        res = inet_ntop(AF_INET6, &a->s6.sin6_addr, ip, INET6_ADDRSTRLEN);
        if (res != NULL) {
            return ntohs(a->s6.sin6_port); 
        }
        return -1;
    }

    return -1;
}

// socket_pton - ip -> AF_INET a->s4.sin_addr or AF_INET6 a->s6.sin6_addr
int socket_pton(sockaddr_t a, int family, char ip[INET6_ADDRSTRLEN], uint16_t port) {
    int res;
    
    if (family == AF_INET) {
        memset(a, 0, sizeof(sockaddr_t));
        res = inet_pton(AF_INET, ip, &a->s4.sin_addr);
        if (res == 1) {
            a->s.sa_family = AF_INET;
            a->len = sizeof(a->s4);
            a->s4.sin_port = htons(port);
        }
        return res;
    }

    if (family == 0 || family == AF_INET6 || family == AF_UNSPEC) {
        memset(a, 0, sizeof(sockaddr_t));
        // 默认按照 ipv6 去处理
        res = inet_pton(AF_INET6, ip, &a->s6.sin6_addr);
        if (res == 1) {
            a->s.sa_family = AF_INET6;
            a->len = sizeof(a->s6);
            a->s6.sin6_port = htons(port);
        }
        return res;
    }

    return -1;
}

socket_t socket_sockaddr_stream(sockaddr_t a, int family) {
    socket_t s;
    assert(family == AF_INET || family == 0 || family == AF_INET6 || family == AF_UNSPEC);
    
    if (family == AF_INET) {
        s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            return INVALID_SOCKET;
        }
        
        memset(a, 0, sizeof(struct sockaddr_in6));
        a->s.sa_family = AF_INET;
        a->len = sizeof(struct sockaddr_in);
        return s;
    }

    if (family == 0 || family == AF_INET6 || family == AF_UNSPEC) { 
        s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            return INVALID_SOCKET;
        }
        
        // 默认走 :: in6addr_any 默认地址
        memset(a, 0, sizeof(struct sockaddr_in6));
        a->s.sa_family = AF_INET6;
        a->len = sizeof(struct sockaddr_in6);
        return s;
    }

    return INVALID_SOCKET;
}

// socket_addr - 通过 family AF_INET or AF_INET6, ip, port 构造 ip sock addr 结构
//               return -1 is error
//               host is char ip[INET6_ADDRSTRLEN] or node host name
int socket_sockaddr(sockaddr_t a, const char * host, uint16_t port, int family) {
    int res;
    char ports[sizeof "65535"];

    res = socket_pton(a, family, (char *)host, port);
    if (res == 1) {
        return 0;
    }

    // 再次通过网络转换
    sprintf(ports, "%hu", port);
    // 这里默认走 TCP 数据流
    struct addrinfo * rsp, req = {
        .ai_family   = family,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
    };

    if (getaddrinfo(host, ports, &req, &rsp)) {
        return -1;
    }

    memset(a, 0, sizeof(sockaddr_t));
    // 尝试获取默认第一个
    memcpy(&a->s, rsp->ai_addr, rsp->ai_addrlen);
    a->len = rsp->ai_addrlen;
    freeaddrinfo(rsp);

    return 0;
}

socket_t 
socket_binds(const char * host, uint16_t port, uint8_t protocol, int * family) {
    socket_t fd;
    char ports[sizeof "65535"];
    // 构建 getaddrinfo 请求参数, ipv6 兼容 ipv4
    struct addrinfo * rsp, req = {
        .ai_socktype = protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM,
        .ai_protocol = protocol,
    };
    if (family != NULL && (*family == AF_INET || *family == AF_INET6)) {
        req.ai_family = *family;
    } else {
        req.ai_family = AF_UNSPEC;
    }

    sprintf(ports, "%hu", port);
    if (getaddrinfo(host, ports, &req, &rsp))
        return INVALID_SOCKET;

    fd = socket(rsp->ai_family, rsp->ai_socktype, 0);
    if (fd == INVALID_SOCKET)
        goto err_free;
    if (socket_set_reuse(fd))
        goto err_close;
    if (bind(fd, rsp->ai_addr, (int)rsp->ai_addrlen))
        goto err_close;

    // Success return ip family
    if (family) {
        *family = rsp->ai_family;
    }
    freeaddrinfo(rsp);
    return fd;

err_close:
    socket_close(fd);
err_free:
    freeaddrinfo(rsp);
    return INVALID_SOCKET;
}

socket_t 
socket_listen(const char * ip, uint16_t port, int backlog) {
    socket_t fd = socket_binds(ip, port, IPPROTO_TCP, NULL);
    if (INVALID_SOCKET != fd && listen(fd, backlog)) {
        socket_close(fd);
        return INVALID_SOCKET;
    }
    return fd;
}

// socket_recvn - socket 接受 sz 个字节
int 
socket_recvn(socket_t s, void * buf, int sz) {
    int r, n = sz;
    while (n > 0 && (r = recv(s, buf, n, 0)) != 0 ) {
        if (r == SOCKET_ERROR) {
            if (errno == EINTR)
                continue;
            return SOCKET_ERROR;
        }
        n -= r;
        buf = (char *)buf + r;
    }
    return sz - n;
}

// socket_sendn - socket 发送 sz 个字节
int 
socket_sendn(socket_t s, const void * buf, int sz) {
    int r, n = sz;
    while (n > 0 && (r = send(s, buf, n, 0)) != 0) {
        if (r == SOCKET_ERROR) {
            if (errno == EINTR)
                continue;
            return SOCKET_ERROR;
        }
        n -= r;
        buf = (char *)buf + r;
    }
    return sz - n;
}

// socket_connect_timeout_partial 带毫秒超时的 connect, 返回非阻塞 socket
static int socket_connect_timeout_partial(socket_t s, const sockaddr_t a, int ms) {
    int n, r;
    struct timeval timeout;
    fd_set rset, wset, eset;

    // 还是阻塞的 connect
    if (ms < 0) return connect(s, &a->s, a->len);

    // 非阻塞登录, 先设置非阻塞模式
    r = socket_set_nonblock(s);
    if (r < 0) return r;

    // 尝试连接, connect 返回 -1 并且 errno == EINPROGRESS 表示正在建立链接
    r = connect(s, &a->s, a->len);
    // connect 链接中, linux 是 EINPROGRESS，winds 是 WSAEWOULDBLOCK
    if (r >= 0 || errno != EINPROGRESS) return r;

    // 超时 timeout, 直接返回结果 -1 错误
    if (ms == 0) return -1;

    FD_ZERO(&rset); FD_SET(s, &rset);
    FD_ZERO(&wset); FD_SET(s, &wset);
    FD_ZERO(&eset); FD_SET(s, &eset);
    timeout.tv_sec = ms / 1000;
    timeout.tv_usec = (ms % 1000) * 1000;
    n = select((int)s + 1, &rset, &wset, &eset, &timeout);
    // 超时返回错误, 防止客户端继续三次握手
    if (n <= 0) return -1;

    // 当连接成功时候,描述符会变成可写
    if (n == 1 && FD_ISSET(s, &wset)) return 0;

    // 当连接建立遇到错误时候, 描述符变为即可读又可写
    if (FD_ISSET(s, &eset) || n == 2) {
        // 只要最后没有 error 那就链接成功
        if (!socket_get_error(s))
            r = 0;
    }

    return r;
}

socket_t 
socket_connect_timeout(const sockaddr_t a, int ms) {
    // 获取 tcp socket 尝试 parse connect
    socket_t s = socket(a->s.sa_family, SOCK_STREAM, IPPROTO_TCP);
    if (s != INVALID_SOCKET) {
        if (socket_connect_timeout_partial(s, a, ms) >= 0) {
            if (socket_set_block(s) >= 0)
                return s;
        } 

        // 构造 connect 失败日志
        char ip[INET6_ADDRSTRLEN];
        int port = socket_ntop(a, ip);
        CERR("ip = %s, port = %d, ms = %d", ip, port, ms);

        socket_close(s);
    }

    return INVALID_SOCKET;
}

socket_t 
socket_connect(const sockaddr_t a) {
    socket_t s = socket(a->s.sa_family, SOCK_STREAM, IPPROTO_TCP);
    if (s != INVALID_SOCKET) {
        if (connect(s, &a->s, a->len) >= 0) {
            return s;
        }

        // 构造 connect 失败日志
        char ip[INET6_ADDRSTRLEN];
        int port = socket_ntop(a, ip);
        CERR("ip = %s, port = %d", ip, port);

        socket_close(s);
    }

    return INVALID_SOCKET;
}
