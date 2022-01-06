#include <spoll.h>

struct userdata {
    socket_t fd;
};

//
// test socket poll 模型
//
void spoll_test(void) {
    const char * host = "127.0.0.1";
    uint16_t port = 8964;

    // 开始构建一个 socket
    socket_t s = socket_listen(host, port, SOMAXCONN);
    if (INVALID_SOCKET == s)
        RETNIL("socket_listen is error!");

    spoll_t p = spoll_create();
    assert(!spoll_invalid(p));

    struct userdata user = { .fd = s };
    if (spoll_add(p, s, &user))
        CERR("spoll_add sock = is error!");
    else {
        spoll_event_t e;
        // 开始等待数据
        printf("spoll_wait [%s:%hu] listen ... \n", host, port);
        int n = spoll_wait(p, e);
        printf("spoll_wait n = %d. 一切都有点点意外!\n", n);

        for (int i = 0; i < n; i++) {
            printf("i = %d, user = %p, u = %p\n", i, &user, e[i].u);
        }
    }

    spoll_delete(p);
    socket_close(s);
}
