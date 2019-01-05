#include "pipe.h"

#ifdef _MSC_VER

//
// pipe - 移植 linux 函数, 通过 WinSock 实现
// pipefd   : 索引 0 表示 recv fd, 1 是 send fd
// return   : 0 is success ,-1 is error returned
//
int 
pipe(socket_t pipefd[2]) {
    socket_t s = socket_stream();
    sockaddr_t name = { AF_INET };
    if (s == INVALID_SOCKET)
        return -1;
    if (socket_bind(s, name))
        goto err_close;
    if (listen(s, 1))
        goto err_close;

    // 得到绑定端口本地地址
    if (socket_getsockname(s, name))
        goto err_close;

    // 开始构建互相通信的 socket
    if ((pipefd[0] = socket_stream()) == INVALID_SOCKET)
        goto err_close;
    if (socket_connect(pipefd[0], name))
        goto err_pipe;

    // 通过 accept 通信避免一些意外
    if ((pipefd[1] = socket_accept(s, name)) == INVALID_SOCKET) 
        goto err_pipe;
    socket_close(s);
    return 0;

err_pipe:
    socket_close(pipefd[0]);
err_close:
    socket_close(s);
    return -1;
}

//
// pipe_open - pipe open
// ch       : 管道类型
// return   : 0 is success ,-1 is error returned
//
inline int 
pipe_open(pipe_t ch) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    return CreatePipe(&ch->recv, &ch->send, &sa, 0) ? 0 : -1;
}

// pipe_recv - 管道阻塞接收, PIPE_BUF 4K 内原子交换
// pipe_send - 管道阻塞发送
inline int 
pipe_recv(pipe_t ch, void * buf, int sz) {
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

#else

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
pipe_recv(pipe_t ch, void * buf, int sz) {
    return (int)read(ch->recv, buf, sz);
}

inline int 
pipe_send(pipe_t ch, const void * buf, int sz) {
    return (int)write(ch->send, buf, sz);
}

#endif
