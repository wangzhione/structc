#include <spoll.h>

struct userdata {
    socket_t fd;
};

#define HOST_STR "127.0.0.1:8964"

//
// test socket poll 模型
//
void spoll_test(void) {
    // 开始构建一个 socket
    socket_t s = socket_tcp(HOST_STR);
    if (INVALID_SOCKET == s)
        RETNIL("socket_tcp is error!");

    poll_t p = s_create();
    assert(!s_invalid(p));

    struct userdata user = { .fd = s };
    if (s_add(p, s, &user))
        CERR("sp_add sock = is error!");
    else {
        event_t e;
        // 开始等待数据
        printf("sp_wait [%s] listen ... \n", HOST_STR);
        int n = s_wait(p, e);
        printf("sp_wait n = %d. 一切都有点点意外!\n", n);

        for (int i = 0; i < n; i++) {
            printf("i = %d, user = %p, u = %p\n", i, &user, e[i].u);
        }
    }

    s_delete(p);
    socket_close(s);
}
