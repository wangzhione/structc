#include "pipe.h"

// pipe_test - test pipe
void pipe_test(void) {
    pipe_t ch;
    SOCKET fd[2];
    char data[] = "我爱传承, I support 祖国.";
    int r = puts(data);
    printf("r = %2d, data = %s\n", r, data);
    IF(pipe_open(ch));

    r = pipe_send(ch, data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);

    r = pipe_recv(ch, data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);

    pipe_close(ch);

    // 这里继续测试 pipe 管道移植版本的兼容性
    IF(pipe(fd));

    r = socket_send(fd[1], data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);
    r = socket_recv(fd[0], data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);

    closesocket(fd[0]); closesocket(fd[1]);
}

inline SOCKET socket_accept(SOCKET s, sockaddr_t a) {
    a->len = sizeof(struct sockaddr_in6);
    return accept(s, &a->s, &a->len);
}

//
// pipe - 移植 linux 函数, 通过 WinSock 实现
// pipefd   : 索引 0 表示 recv fd, 1 是 send fd
// return   : 0 is success -1 is error returned
//
int pipe_socket(SOCKET pipefd[2]) {
    sockaddr_t name;
    SOCKET s = socket_sockaddr_stream(name, AF_INET6);
    if (s == INVALID_SOCKET)
        return -1;
    
    // 绑定默认网卡, 多平台上更容易 connect success
    name->s6.sin6_addr = in6addr_loopback;
    
    if (bind(s, &name->s, name->len)) 
        goto err_close;

    if (listen(s, 1))
        goto err_close;

    // 得到绑定端口和本地地址
    if (getsockname(s, &name->s, &name->len))
        goto err_close;

    // 开始构建互相通信的 socket
    pipefd[0] = socket_connect(name);
    if (pipefd[0] == INVALID_SOCKET)
        goto err_close;

    // 通过 accept 通信避免一些意外
    pipefd[1] = socket_accept(s, name);
    if (pipefd[1] == INVALID_SOCKET) 
        goto err_pipe;

    closesocket(s);
    return 0;
err_pipe:
    closesocket(pipefd[0]);
err_close:
    closesocket(s);
    return -1;
}

int _pipe3(SOCKET pipefd[2]) {
    SOCKET s;
    sockaddr_t name = { {.s = { AF_INET }, .len = sizeof(struct sockaddr_in) } };
    socklen_t nlen = name->len;
    name->s4.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // 开启一个固定 socket
    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        return -1;

    if (bind(s, &name->s, nlen))
        return closesocket(s), -1;
    if (listen(s, 1))
        return closesocket(s), -1;

    // 得到绑定的数据
    if (getsockname(s, &name->s, &nlen))
        return closesocket(s), -1;

    // 开始构建互相通信的socket
    if ((pipefd[0] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        return closesocket(s), -1;

    if (connect(pipefd[0], &name->s, nlen))
        return closesocket(s), -1;

    // 可以继续添加, 通信协议来避免一些意外
    if ((pipefd[1] = accept(s, &name->s, &nlen)) == INVALID_SOCKET)
        return closesocket(pipefd[0]), closesocket(s), -1;

    return closesocket(s), 0;
}

int _pipe4(SOCKET pipefd[2]) {
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

void pipe_socket_test(void) {
    int r;
    SOCKET pipefd[2];
    IF(_pipe4(pipefd));

    char data[] = "君子和而不同, I support 自由.";

    r = socket_send(pipefd[1], data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);
    r = socket_recv(pipefd[0], data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);

    closesocket(pipefd[0]);
    closesocket(pipefd[1]);
}
