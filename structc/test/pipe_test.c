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
