#include <socket.h>

#ifdef _MSC_VER

//
// gettimeofday - Linux sys/time.h 中得到微秒时间实现
// tv		:	返回结果包含秒数和微秒数
// tz		:	包含的时区, winds 上这个变量没有作用
// return	:   默认返回 0
//
int 
gettimeofday(struct timeval * tv, void * tz) {
    struct tm m;
    SYSTEMTIME se;

    GetLocalTime(&se);
    m.tm_year = se.wYear - 1900;
    m.tm_mon = se.wMonth - 1;
    m.tm_mday = se.wDay;
    m.tm_hour = se.wHour;
    m.tm_min = se.wMinute;
    m.tm_sec = se.wSecond;
    m.tm_isdst = -1; // 不考虑夏令时

    tv->tv_sec = (long)mktime(&m);
    tv->tv_usec = se.wMilliseconds * 1000;

    return 0;
}

#endif

//
// socket_recv      - socket 接受
// socket_recvn     - socket 接受 sz 个字节
// socket_send      - socket 发送
// socket_sendn     - socket 发送 sz 个字节
//
int 
socket_recv(socket_t s, void * buf, int sz) {
    int r;
    do {
        r = recv(s, buf, sz, 0);
    } while (errno == EINTR && r == SOCKET_ERROR);
    return r;
}

int 
socket_recvn(socket_t s, void * buf, int sz) {
    int r, n = sz;
    while (n > 0) {
        r = socket_recv(s, buf, n);
        if (r == 0) break;
        if (r == SOCKET_ERROR)
            return SOCKET_ERROR;
        n -= r;
        buf = (char *)buf + r;
    }
    return sz - n;
}

int 
socket_send(socket_t s, const void * buf, int sz) {
    int r;
    do {
        r = send(s, buf, sz, 0);
    } while (errno == EINTR && r == SOCKET_ERROR);
    return r;
}

int 
socket_sendn(socket_t s, const void * buf, int sz) {
    int r, n = sz;
    while (n > 0) {
        r = socket_send(s, buf, n);
        if (r == 0) break;
        if (r == SOCKET_ERROR)
            return SOCKET_ERROR;
        n -= r;
        buf = (char *)buf + r;
    }
    return sz - n;
}

//
// socket_addr - 通过 ip, port 构造 ipv4 结构
//
int 
socket_addr(const char * ip, uint16_t port, sockaddr_t * addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip);
    if (addr->sin_addr.s_addr == INADDR_NONE) {
        struct hostent * host = gethostbyname(ip);
        if (!host || !host->h_addr)
            return EParam;

        // 尝试一种, 默认 ipv4
        memcpy(&addr->sin_addr, host->h_addr, host->h_length);
    }
    memset(addr->sin_zero, 0, sizeof addr->sin_zero);

    return SBase;
}

//
// socket_connecto      - connect 超时链接, 返回非阻塞 socket
//
int
socket_connecto(socket_t s, const sockaddr_t * addr, int ms) {
    int n, r;
    struct timeval to;
    fd_set rset, wset, eset;

    // 还是阻塞的connect
    if (ms < 0) return socket_connect(s, addr);

    // 非阻塞登录, 先设置非阻塞模式
    r = socket_set_nonblock(s);
    if (r < SBase) return r;

    // 尝试连接, connect 返回 -1 并且 errno == EINPROGRESS 表示正在建立链接
    r = socket_connect(s, addr);
    // connect 链接中, linux 是 EINPROGRESS，winds 是 WSAEWOULDBLOCK
    if (r >= SBase || errno != ECONNECT) 
        return r;

    // 超时 timeout, 直接返回结果 EBase = -1 错误
    if (ms == 0) return EBase;

    FD_ZERO(&rset); FD_SET(s, &rset);
    FD_ZERO(&wset); FD_SET(s, &wset);
    FD_ZERO(&eset); FD_SET(s, &eset);
    to.tv_sec = ms / 1000;
    to.tv_usec = (ms % 1000) * 1000;
    n = select((int)s + 1, &rset, &wset, &eset, &to);
    // 超时直接滚
    if (n <= 0) return EBase;

    // 当连接成功时候,描述符会变成可写
    if (n == 1 && FD_ISSET(s, &wset))
        return SBase;

    // 当连接建立遇到错误时候, 描述符变为即可读又可写
    if (FD_ISSET(s, &eset) || n == 2) {
        socklen_t len = sizeof n;
        // 只要最后没有 error那就 链接成功
        if (!getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&n, &len) && !n)
            return SBase;
    }

    return EBase;
}

//
// socket_bind      - 端口绑定返回绑定好的 socket fd, 返回 INVALID_SOCKET or PF_INET PF_INET6
// socket_listen    - 端口监听返回监听好的 socket fd.
//
socket_t 
socket_bind(const char * ip, uint16_t port, uint8_t protocol, int * family) {
    socket_t fd;
    char ports[sizeof "65535"];
    struct addrinfo hints = { 0 };
    struct addrinfo * addrs = NULL;
    if (NULL == ip || *ip == '\0')
        ip = "0.0.0.0"; // INADDR_ANY

    sprintf(ports, "%hu", port);
    hints.ai_family = AF_UNSPEC;
    if (protocol == IPPROTO_TCP)
        hints.ai_socktype = SOCK_STREAM;
    else {
        assert(protocol == IPPROTO_UDP);
        hints.ai_socktype = SOCK_DGRAM;
    }
    hints.ai_protocol = protocol;

    if (getaddrinfo(ip, ports, &hints, &addrs))
        return INVALID_SOCKET;

    fd = socket(addrs->ai_family, addrs->ai_socktype, 0);
    if (fd == INVALID_SOCKET)
        goto _fail;
    if (socket_set_reuseaddr(fd))
        goto _faed;
    if (bind(fd, addrs->ai_addr, (int)addrs->ai_addrlen))
        goto _faed;

    // Success return ip family
    if (family)
        *family = addrs->ai_family;

    freeaddrinfo(addrs);
    return fd;

_faed:
    socket_close(fd);
_fail:
    freeaddrinfo(addrs);
    return INVALID_SOCKET;
}

socket_t 
socket_listen(const char * ip, uint16_t port, int backlog) {
    socket_t fd = socket_bind(ip, port, IPPROTO_TCP, NULL);
    if (INVALID_SOCKET != fd && listen(fd, backlog)) {
        socket_close(fd);
        return INVALID_SOCKET;
    }
    return fd;
}
