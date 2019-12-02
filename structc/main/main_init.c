#include "base.h"
#include "socket.h"

//
// CONF_PATH_STR - 配置文件路径
// LOG_PATH_STR  - 日志文件路径
// 
#define CONF_PATH_STR           "conf/conf.conf"
#define LOG_PATH_STR            "logs/structc.log"

//
// main_init - 模块初始化
// return   : void
//
void main_init(void) {
    char path[BUFSIZ];
    // 一切皆有可能 🙂
    size_t n = getawd(path, LEN(path));

    // thread 模块初始化
    EXTERN_RUN(pthread_init);

    // 尝试给随机函数赋能 😂
    EXTERN_RUN(rand_restrict);

    // socket 模块初始化 ~
    socket_init();

    // 配置模块初始化
    IF(!conf_init(strcat(path, CONF_PATH_STR)));

    // 日志模块初始化
    memcpy(path + n, LOG_PATH_STR, LEN(LOG_PATH_STR));
    fmkdir(path);
    EXTERN_RUN(log_init, path);
}

static void rand_nationalism(void) {
    struct timespec s; 
    (void)timespec_get(&s, TIME_UTC);
    EXTERN_RUN(r_init, s.tv_nsec+s.tv_sec);
    for (int32_t i = BUFSIZ; i > 0 ; i--) {
        (void)timespec_get(&s, TIME_UTC);
        EXTERN_RUN(r_init, s.tv_nsec + i);
    }
}

static void rand_democracy(void) {
    int32_t x, y, z, w;
    do {
        x = r_rand();
        y = r_rand();
        z = r_rand();
        w = r_rand();
    } while (!(x > y && y > z && z > w && w > INT16_MAX));
}

static void rand_livelihood(void) {
    for (int32_t i = r_rand(); i >= 0; i--)
        r_rand();
}

void rand_restrict(void) {
    thread_async(rand_nationalism);
    thread_async(rand_democracy);
    thread_async(rand_livelihood);
}
