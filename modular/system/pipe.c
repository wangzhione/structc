#include "pipe.h"

#if defined(__linux__) && defined(__GNUC__)

//
// pipe_open - pipe open
// ch       : 管道类型
// return   : 0 is success ,-1 is error returned
//
inline int 
pipe_open(pipe_t ch) {
    return pipe(&ch->recv);
}

// pipe_recv - 管道阻塞接收, PIPE_BUF 4K 内原子交换
// pipe_send - 管道阻塞发送
inline int 
pipe_recv(pipe_t ch, void * __restrict buf, int sz) {
    return (int)read(ch->recv, buf, sz);
}

inline int 
pipe_send(pipe_t ch, const void * buf, int sz) {
    return (int)write(ch->send, buf, sz);
}

#elif defined(_WIN32) && defined(_MSC_VER)

//
// pipe - 移植 linux 函数, 通过 WinSock 实现
// pipefd   : 索引 0 表示 recv fd, 1 是 send fd
// return   : 0 is success -1 is error returned
//
int pipe(SOCKET pipefd[2]) {
    struct sockaddr_in6 name;
    socklen_t len = sizeof(struct sockaddr_in6);

    SOCKET s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        PERR("soccket AF_INET6 SOCK_STREAM error");
        return -1;
    }

    memset(&name, 0, sizeof(struct sockaddr_in6));
    name.sin6_family = AF_INET6;
    // 绑定默认网卡, 多平台上更容易 connect success
    name.sin6_addr = in6addr_loopback;
    if (bind(s, (struct sockaddr *)&name, len)) {
        PERR("bind in6addr_loopback error");
        goto fail_socket;
    }

    // 开始监听
    if (listen(s, 1)) {
        PERR("listen backlog = 1 error");
        goto fail_socket;
    }

    // 得到 server socket 绑定端口和本地地址
    if (getsockname(s, (struct sockaddr *)&name, &len)) {
        PERR("getsockname sockaddr error");
        goto fail_socket;
    }

    // 开始尝试构建 client socket connect server socket
    pipefd[0] = socket(name.sin6_family, SOCK_STREAM, IPPROTO_TCP);
    if (pipefd[0] == INVALID_SOCKET) {
        PERR("socket client error");
        goto fail_socket;
    }
    if (connect(pipefd[0], (struct sockaddr *)&name, len)) {
        PERR("connect error");
        goto fail_pipe;
    }
    // 准备 accept 建立链接
    pipefd[1] = accept(s, (struct sockaddr *)&name, &len);
    if (pipefd[1] == INVALID_SOCKET) {
        PERR("accept error");
        goto fail_pipe;
    }

    // pipefd[0] recv fd, pipefd[1] send fd
    shutdown(pipefd[0], SHUT_WR);
    shutdown(pipefd[1], SHUT_RD);

    closesocket(s);
    return 0;
fail_pipe:
    closesocket(pipefd[0]);
fail_socket:
    closesocket(s);
    return -1;
}

//
// pipe_open - pipe open
// ch       : 管道类型
// return   : 0 is success ,-1 is error returned
//
inline int 
pipe_open(pipe_t ch) {
    SECURITY_ATTRIBUTES a = { 
        .nLength              = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = NULL,
        .bInheritHandle       = TRUE,
    };
    return CreatePipe(&ch->recv, &ch->send, &a, 0) ? 0 : -1;
}

// pipe_recv - 管道阻塞接收, PIPE_BUF 4K 内原子交换
// pipe_send - 管道阻塞发送
inline int 
pipe_recv(pipe_t ch, void * __restrict buf, int sz) {
    DWORD len = 0;
    BOOL ret = PeekNamedPipe(ch->recv, NULL, 0, NULL, &len, NULL);
    if (!ret || len <= 0)
        return -1;

    // 开始读取数据
    ret = ReadFile(ch->recv, buf, sz, &len, NULL);
    return ret ? (int)len : -1;
}

inline int 
pipe_send(pipe_t ch, const void * buf, int sz) {
    DWORD len = 0;
    if (WriteFile(ch->send, buf, sz, &len, NULL))
        return (int)len;
    return -1;
}

#endif
