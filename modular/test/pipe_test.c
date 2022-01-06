#include <pipe.h>

// pipe_test - test pipe
void pipe_test(void) {
    pipe_t ch;
    socket_t fd[2];
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

    socket_close(fd[0]); socket_close(fd[1]);
}

//
// pipe - 移植 linux 函数, 通过 WinSock 实现
// pipefd   : 索引 0 表示 recv fd, 1 是 send fd
// return   : 0 is success -1 is error returned
//
int pipe_socket(socket_t pipefd[2]) {
    sockaddr_t name;
    socket_t s = socket_sockaddr_stream(name, AF_INET6);
    if (s == INVALID_SOCKET)
        return -1;
    
    // 绑定默认网卡, 多平台上更容易 connect success
    name->s6.sin6_addr = in6addr_loopback;
    
    if (bind(s, &name->s, name->len)) 
        goto err_close;

    if (listen(s, 1))
        goto err_close;

    // 得到绑定端口和本地地址
    if (socket_getsockname(s, name))
        goto err_close;

    // 开始构建互相通信的 socket
    pipefd[0] = socket_connect(name);
    if (pipefd[0] == INVALID_SOCKET)
        goto err_close;

    // 通过 accept 通信避免一些意外
    pipefd[1] = socket_accept(s, name);
    if (pipefd[1] == INVALID_SOCKET) 
        goto err_pipe;

    socket_close(s);
    return 0;
err_pipe:
    socket_close(pipefd[0]);
err_close:
    socket_close(s);
    return -1;
}

int pipe3(socket_t pipefd[2]) {
    socket_t s;
    sockaddr_t name = { {.s = { AF_INET }, .len = sizeof(struct sockaddr_in) } };
    socklen_t nlen = name->len;
    name->s4.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // 开启一个固定 socket
    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        return -1;

    if (bind(s, &name->s, nlen))
        return socket_close(s), -1;
    if (listen(s, 1))
        return socket_close(s), -1;

    // 得到绑定的数据
    if (getsockname(s, &name->s, &nlen))
        return socket_close(s), -1;

    // 开始构建互相通信的socket
    if ((pipefd[0] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        return socket_close(s), -1;

    if (connect(pipefd[0], &name->s, nlen))
        return socket_close(s), -1;

    // 可以继续添加, 通信协议来避免一些意外
    if ((pipefd[1] = accept(s, &name->s, &nlen)) == INVALID_SOCKET)
        return socket_close(pipefd[0]), socket_close(s), -1;

    return socket_close(s), 0;
}

void pipe_socket_test(void) {
    int r;
    socket_t pipefd[2];
    IF(pipe_socket(pipefd));

    char data[] = "君子和而不同, I support 自由.";

    r = socket_send(pipefd[1], data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);
    r = socket_recv(pipefd[0], data, sizeof data);
    printf("r = %2d, data = %s\n", r, data);

    socket_close(pipefd[0]); 
    socket_close(pipefd[1]);
}