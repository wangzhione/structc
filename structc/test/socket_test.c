#include <socket.h>

//
// socket test 功能
//
void socket_test(void) {
    int r;
    sockaddr_t addr;
    const char * host;

    host = "127.0.0.1:19527";
    r = socket_host(host, addr);
    if (r < SBase)
        RETNIL("socket_host is err %s", host);

    // 输出 addr
    printf("family = %d, sin_port = %d\n", 
        addr->sin_family, addr->sin_port);
}
