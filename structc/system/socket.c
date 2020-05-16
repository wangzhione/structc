#include "socket.h"

// socket_recvn - socket 接受 sz 个字节
int 
socket_recvn(socket_t s, void * buf, int sz) {
    int r, n = sz;
    while (n > 0 && (r = recv(s, buf, n, 0)) > 0 ) {
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
    while (n > 0 && (r = send(s, buf, n, 0)) > 0) {
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

// socket_addr - 通过 ip, port 构造 ipv4 结构
int socket_addr(char ip[INET6_ADDRSTRLEN], uint16_t port, sockaddr_t a) {
    a->sin_addr.s_addr = inet_addr(ip);
    if (a->sin_addr.s_addr != INADDR_NONE) {
        a->sin_family = AF_INET;
        a->sin_port = htons(port);
        memset(a->sin_zero, 0, sizeof a->sin_zero);
    } else {
        char ports[sizeof "65535"]; sprintf(ports, "%hu", port);
        struct addrinfo * rsp, req = {
            .ai_family   = PF_INET,
            .ai_socktype = SOCK_STREAM,
        };

        if (getaddrinfo(ip, ports, &req, &rsp)) {
            return -1;
        }

        // 尝试默认第一个 ipv4
        memcpy(a, rsp->ai_addr, rsp->ai_addrlen);
        freeaddrinfo(rsp);
    }

    return 0;
}

// socket_binds - 返回绑定好端口的 socket fd, family is PF_INET PF_INET6
socket_t 
socket_binds(const char * ip, uint16_t port, uint8_t protocol, int * family) {
    // 构建 getaddrinfo 请求参数
    if (!ip || !*ip) ip = "0.0.0.0";
    char ports[sizeof "65535"]; sprintf(ports, "%hu", port);
    struct addrinfo * rsp, req = {
        .ai_family   = PF_UNSPEC,
        .ai_socktype = protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM,
        .ai_protocol = protocol,
    };
    if (getaddrinfo(ip, ports, &req, &rsp))
        return INVALID_SOCKET;

    socket_t fd = socket(rsp->ai_family, rsp->ai_socktype, 0);
    if (fd == INVALID_SOCKET)
        goto err_free;
    if (socket_set_reuse(fd))
        goto err_close;
    if (bind(fd, rsp->ai_addr, (int)rsp->ai_addrlen))
        goto err_close;

    // Success return ip family
    if (family) *family = rsp->ai_family;
    freeaddrinfo(rsp);
    return fd;

err_close:
    socket_close(fd);
err_free:
    freeaddrinfo(rsp);
    return INVALID_SOCKET;
}

// socket_listens - 返回监听好的 socket fd
socket_t 
socket_listens(const char * ip, uint16_t port, int backlog) {
    socket_t fd = socket_binds(ip, port, IPPROTO_TCP, NULL);
    if (INVALID_SOCKET != fd && listen(fd, backlog)) {
        socket_close(fd);
        return INVALID_SOCKET;
    }
    return fd;
}

// host_parse - 解析 host 内容
static int host_parse(const char * host, char ip[INET6_ADDRSTRLEN]) {
    int port = 0;
    char * begin = ip;
    if (!host || !*host || *host == ':')
        strcpy(ip, "0.0.0.0");
    else {
        char c;
        // 简单检查字符串是否合法
        size_t n = strlen(host);
        if (n >= INET6_ADDRSTRLEN) {
            RETERR("host err %s", host);
        }

        // 寻找分号
        while ((c = *host++) != ':' && c)
            *ip++ = c;
        *ip = '\0';
        if (c == ':') {
            if (n > ip - begin + sizeof "65535") {
                RETERR("host port err %s", host);
            }
            port = atoi(host);
        }
    }

    return port;
}

//
// socket_host - 通过 ip:port 串得到 socket addr 结构
// host     : ip:port 串
// a        : 返回最终生成的地址
// return   : >= 0 表示成功
//
int 
socket_host(const char * host, sockaddr_t a) {
    char ip[INET6_ADDRSTRLEN];
    int port = host_parse(host, ip);
    if (port < 0)
        return port;

    // 开始构造 sockaddr
    if (!a) {
        sockaddr_t addr;
        return socket_addr(ip, port, addr);
    }
    return socket_addr(ip, port, a);
}

//
// socket_tcp - 创建 TCP 详细套接字
// host     : ip:port 串  
// return   : 返回监听后套接字
//
socket_t 
socket_tcp(const char * host) {
    char ip[INET6_ADDRSTRLEN];
    int port = host_parse(host, ip);
    if (port < 0)
        RETURN(INVALID_SOCKET, "host_parse %d, %s", port, host);
    return socket_listens(ip, port, SOMAXCONN);
}

//
// socket_udp - 创建 UDP 详细套接字
// host     : ip:port 串  
// return   : 返回绑定后套接字
//
socket_t 
socket_udp(const char * host) {
    char ip[INET6_ADDRSTRLEN];
    int port = host_parse(host, ip);
    if (port < 0)
        RETURN(INVALID_SOCKET, "host_parse %d, %s", port, host);
    return socket_binds(ip, port, IPPROTO_UDP, NULL);
}

//
// socket_connects - 返回链接后的阻塞套接字
// host     : ip:port 串  
// return   : 返回链接后阻塞套接字
//
socket_t 
socket_connects(const char * host) {
    sockaddr_t a;
    // 先解析 sockaddr 地址
    if (socket_host(host, a) < 0)
        return INVALID_SOCKET;

    // 获取 tcp socket 尝试 parse connect
    socket_t s = socket_stream();
    if (s != INVALID_SOCKET) {
        if (socket_connect(s, a) >= 0)
            return s;

        socket_close(s);
    }

    RETURN(INVALID_SOCKET, "host = %s", host);
}

// socket_connecto - connect 带超时的链接, 返回非阻塞 socket
static int socket_connecto(socket_t s, const sockaddr_t a, int ms) {
    int n, r;
    struct timeval timeout;
    fd_set rset, wset, eset;

    // 还是阻塞的connect
    if (ms < 0) return socket_connect(s, a);

    // 非阻塞登录, 先设置非阻塞模式
    r = socket_set_nonblock(s);
    if (r < 0) return r;

    // 尝试连接, connect 返回 -1 并且 errno == EINPROGRESS 表示正在建立链接
    r = socket_connect(s, a);
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

//
// socket_connectos - 返回链接后的阻塞套接字
// host     : ip:port 串  
// ms       : 链接过程中毫秒数
// return   : 返回链接后阻塞套接字
//
socket_t 
socket_connectos(const char * host, int ms) {
    sockaddr_t a;
    // 先解析 sockaddr 地址
    if (socket_host(host, a) < 0)
        return INVALID_SOCKET;

    // 获取 tcp socket 尝试 parse connect
    socket_t s = socket_stream();
    if (s != INVALID_SOCKET) {
        if (socket_connecto(s, a, ms) >= 0) {
            // 设置为阻塞套接字
            socket_set_block(s);
            return s;
        }

        socket_close(s);
    }

    RETURN(INVALID_SOCKET, "host = %s", host);
}
