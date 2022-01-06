#pragma once

#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>

#include "system.h"
#include "struct.h"

#if defined(__linux__) && defined(__GNUC__)

#include <netdb.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/resource.h>

// This is used instead of -1, since the. by WinSock
// On now linux EAGAIN and EWOULDBLOCK may be the same value 
// connect 链接中, linux 是 EINPROGRESS，winds 是 WSAEWOULDBLOCK
//
typedef int             socket_t;

#define INVALID_SOCKET  (~0)
#define SOCKET_ERROR    (-1)

// socket_init - 初始化 socket 库初始化方法
inline void socket_init(void) {
    // 防止管道破裂, 忽略 SIGPIPE 信号
    signal(SIGPIPE, SIG_IGN);
    // 防止会话关闭, 忽略 SIGHUP  信号
    signal(SIGHUP , SIG_IGN);
}

inline int socket_close(socket_t s) {
    return close(s);
}

// socket_set_block - 设置套接字是阻塞
inline static int socket_set_block(socket_t s) {
    int mode = fcntl(s, F_GETFL);
    if (mode == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }
    return fcntl(s, F_SETFL, mode & ~O_NONBLOCK);
}

// socket_set_nonblock - 设置套接字是非阻塞
inline int socket_set_nonblock(socket_t s) {
    int mode = fcntl(s, F_GETFL);
    if (mode == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }
    return fcntl(s, F_SETFL, mode | O_NONBLOCK);
}

#elif defined(_WIN32) && defined(_MSC_VER)

#include <ws2tcpip.h>

typedef SOCKET          socket_t;
typedef int             socklen_t;

#undef  errno
#define errno           WSAGetLastError()

#undef  EINTR
#define EINTR           WSAEINTR
#undef  EAGAIN
#define EAGAIN          WSAEWOULDBLOCK
#undef  ETIMEDOUT
#define ETIMEDOUT       WSAETIMEDOUT
#undef  EINPROGRESS
#define EINPROGRESS     WSAEWOULDBLOCK

// WinSock 2 extension manifest constants for shutdown()
//
#define SHUT_RD         SD_RECEIVE
#define SHUT_WR         SD_SEND
#define SHUT_RDWR       SD_BOTH

#define SO_REUSEPORT    SO_REUSEADDR

// socket_init - 初始化 socket 库初始化方法
inline void socket_init(void) {
    WSADATA version;
    IF(WSAStartup(WINSOCK_VERSION, &version));
}

// socket_close - 关闭上面创建后的句柄
inline int socket_close(socket_t s) {
    return closesocket(s);
}

// socket_set_block - 设置套接字是阻塞
inline int socket_set_block(socket_t s) {
    u_long ov = 0;
    return ioctlsocket(s, FIONBIO, &ov);
}

// socket_set_nonblock - 设置套接字是非阻塞
inline int socket_set_nonblock(socket_t s) {
    u_long ov = 1;
    return ioctlsocket(s, FIONBIO, &ov);
}

#endif

// socket_recv - 读取数据
inline int socket_recv(socket_t s, void * buf, int sz) {
    if (likely(sz > 0)) {
        return (int)recv(s, buf, sz, 0);
    }
    return 0;
}

// socket_send - 写入数据
inline int socket_send(socket_t s, const void * buf, int sz) {
    return (int)send(s, buf, sz, 0);
}

inline int socket_set_enable(socket_t s, int optname) {
    int ov = 1;
    return setsockopt(s, SOL_SOCKET, optname, (void *)&ov, sizeof ov);
}

// socket_set_reuse - 开启端口和地址复用
inline int socket_set_reuse(socket_t s) {
    return socket_set_enable(s, SO_REUSEPORT);
}

// socket_set_keepalive - 开启心跳包检测, 默认 5 次/2h
inline int socket_set_keepalive(socket_t s) {
    return socket_set_enable(s, SO_KEEPALIVE);
}

inline int socket_set_time(socket_t s, int ms, int optname) {
    struct timeval ov = { 0,0 };
    if (ms > 0) {
        ov.tv_sec = ms / 1000;
        ov.tv_usec = (ms % 1000) * 1000;
    }
    return setsockopt(s, SOL_SOCKET, optname, (void *)&ov, sizeof ov);
}

// socket_set_rcvtimeo - 设置接收数据毫秒超时时间
inline int socket_set_rcvtimeo(socket_t s, int ms) {
    return socket_set_time(s, ms, SO_RCVTIMEO);
}

// socket_set_sndtimeo - 设置发送数据毫秒超时时间
inline int socket_set_sndtimeo(socket_t s, int ms) {
    return socket_set_time(s, ms, SO_SNDTIMEO);
}

// socket_get_error - 获取 socket error 值, 0 正确, 其它都是 error
inline int socket_get_error(socket_t s) {
    int err, no = errno;
    socklen_t len = sizeof(err);
    return getsockopt(s, SOL_SOCKET, SO_ERROR, (void *)&err, &len) ? no : err;
}

// sockaddr_t 为 socket 默认通用地址结构
// len == sizeof(struct sockaddr_in) => ipv4
// len == sizeof(struct sockaddr_in) => ipv6
typedef struct {
    union {
        struct sockaddr s;
        //
        // s4->sin_family = AF_INET
        // s4->sin_port = htons(9527)
        // inet_pton(AF_INET, "189.164.0.1", &s4->sin_addr) == 1 => success
        //
        struct sockaddr_in s4;
        struct sockaddr_in6 s6;
    };

    socklen_t len;
} sockaddr_t[1];

//
// socket create 
// socket_t s; 
//
// socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP)
// socket(AF_INET6, SOCK_DGRAM , IPPROTO_UDP)
// socket(AF_INET , SOCK_STREAM, IPPROTO_TCP)
// socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)
// 

extern socket_t socket_sockaddr_stream(sockaddr_t a, int family);

extern int socket_sockaddr(sockaddr_t a, const char * host, uint16_t port, int family);

// socket_getsockname - 获取 socket 的本地地址
inline int socket_getsockname(socket_t s, sockaddr_t name) {
    return getsockname(s, &name->s, &name->len);
}

// socket_getpeername - 获取 client socket 的地址
inline int socket_getpeername(socket_t s, sockaddr_t name) {
    return getpeername(s, &name->s, &name->len);
}

// socket_ntop - sin_addr or sin6_addr -> ip 串, return -1 error or port
extern int socket_ntop(const sockaddr_t a, char ip[INET6_ADDRSTRLEN]);

// socket_bind - 返回绑定好端口的 socket fd, family return AF_INET AF_INET6
extern socket_t socket_binds(const char * host, uint16_t port, uint8_t protocol, int * family);

// socket_listen - 返回监听好的 socket fd
extern socket_t socket_listen(const char * ip, uint16_t port, int backlog);

// socket_recvfrom  - recvfrom 接受函数
inline int socket_recvfrom(socket_t s, void * buf, int sz, sockaddr_t in) {
    // ssize_t recvfrom(int sockfd, void * buf, size_t len, int flags,
    //                  struct sockaddr * src_addr, socklen_t * addrlen);
    //
    // If  src_addr  is  not  NULL, and the underlying protocol provides the source
    // address, this source address is filled in.  When src_addr is  NULL,  nothing
    // is  filled  in;  in this case, addrlen is not used, and should also be NULL.
    // The argument addrlen is a value-result argument,  which  the  caller  should
    // initialize  before  the  call  to  the  size  of  the buffer associated with
    // src_addr, and modified on return to indicate the actual size of  the  source
    // address.   The  returned  address is truncated if the buffer provided is too
    // small; in this case, addrlen will return a value greater than  was  supplied
    // to the call.
    in->len = sizeof(in->s6);
    return (int)recvfrom(s, buf, sz, 0, &in->s, &in->len);
}

// socket_sendto    - sendto 发送函数
inline int socket_sendto(socket_t s, const void * buf, int sz, const sockaddr_t to) {
    return (int)sendto(s, buf, sz, 0, &to->s, to->len);
}

// socket_recvn - socket 接受 sz 个字节
extern int socket_recvn(socket_t s, void * buf, int sz);

// socket_sendn - socket 发送 sz 个字节
extern int socket_sendn(socket_t s, const void * buf, int sz);

// socket_connect_timeout - 毫秒超时的 connect
extern socket_t socket_connect_timeout(const sockaddr_t a, int ms);

extern socket_t socket_connect(const sockaddr_t a);

inline socket_t socket_accept(socket_t s, sockaddr_t a) {
    a->len = sizeof(struct sockaddr_in6);
    return accept(s, &a->s, &a->len);
}
