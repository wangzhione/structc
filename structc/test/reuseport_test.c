#include <times.h>
#include <socket.h>
#include <thread.h>

// UINT_PORT - 监听端口
#define UINT_PORT (8088)

// accept_example - SO_REUSEPORT accept example 
void accept_example(int id);

// reuseport_test - 端口复用测试
void reuseport_test(void) {
    // start 10 pthread run accept_example
    for (int i = 1; i <= 10; ++i) {
        IF(pthread_async(accept_example, i));
    }

    msleep(609);

    // main accept block
    accept_example(0);
}

// accept_example - SO_REUSEPORT accept example 
void 
accept_example(int id) {
    // 构造 TCP socket
    socket_t s = socket_stream();
    IF(s == INVALID_SOCKET);

    // 开启地址复用
    IF(socket_set_reuseaddr(s));

    // 构造地址
    sockaddr_t addr = {{ AF_INET, htons(UINT_PORT) }};

    // 绑定地址
    IF(socket_bind(s, addr));

    // 开始监听
    IF(socket_listen(s));

    // 等待链接过来, 最多 3 次
    for (int i = 0; i < 3; ++i) {
        times_t v;
        sockaddr_t cddr;

        printf("[%2d] accept start ... %s\n", id, times_str(v));
        socket_t c = socket_accept(s, cddr);
        if (s == INVALID_SOCKET) {
            CERR("accept s = %lld is error", (int64_t)s);
            break;
        }

        // 连接成功打印链接消息
        printf("[%2d] [%s:%d] accept success ... %s\n", id,
               inet_ntoa(cddr->sin_addr), ntohs(cddr->sin_port),
               times_str(v));

        socket_close(c);
    }

    socket_close(s);
}
