#include <spoll.h>

#define HOST_STR "127.0.0.1:19527"

//
// test socket poll 模型
//
void spoll_test(void) {
    int n;
    event_t e;

    // 开始构建一个 socket
    socket_t s = socket_tcp(HOST_STR);
    if (INVALID_SOCKET == s)
        RETNIL("socket_tcp is error!");

    poll_t p = s_create();
    assert(!s_invalid(p));

    if (s_add(p, s, NULL))
        CERR("sp_add sock = is error!");
    else {
        // 开始等待数据
        printf("sp_wait [%s] listen ... \n", HOST_STR);
        n = s_wait(p, e);
        printf("sp_wait n = %d. 一切都有点点意外!\n", n);
    }

    s_delete(p);
    socket_close(s);
}
