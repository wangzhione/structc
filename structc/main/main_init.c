#include "head.h"
#include "socket.h"

//
// CONF_PATH_STR - 配置文件路径
// LOG_PATH_STR  - 日志文件路径
// 
#define CONF_PATH_STR       "conf/conf.conf"
#define LOG_PATH_STR        "logs/structc.log"

//
// main_init - 模块初始化
// return   : void
//
void main_init(void) {
    char r[BUFSIZ];
    int n = getawd(r, sizeof r);
    assert(0 < n && n < sizeof r);

    // thread 模块初始化
    EXTERN_RUN(pthread_init);

    // 此后 :) 随机与你无关 ~
    EXTERN_RUN(rand_restrict);

    // socket 模块初始化 ~
    socket_init();

    // 配置模块初始化
    IF(!conf_init(strcat(r, CONF_PATH_STR)));

    // 日志模块初始化
    memcpy(r+n, LOG_PATH_STR, LEN(LOG_PATH_STR));
    mkfdir(r);
    EXTERN_RUN(log_init, r);
}

// rand_nationalism - 民族主义, 国家是人民所共有, 各族平等, 团结一致
static void rand_nationalism(void) {
    struct timespec s; 
    (void)timespec_get(&s, TIME_UTC);
    EXTERN_RUN(r_init, s.tv_nsec + s.tv_sec);
    for (int32_t i = BUFSIZ; i > 0 ; --i) {
        (void)timespec_get(&s, TIME_UTC);
        EXTERN_RUN(r_init, s.tv_nsec + i);
    }
}

// rand_democracy - 民权主义, 政治是人民所共管, 选举权、罢免权、创制权和复决权
static void rand_democracy(void) {
    int32_t x, y, z, w;
    do {
        x = r_rand();
        y = r_rand();
        z = r_rand();
        w = r_rand();
    } while (!(x > y && y > z && z > w && w > INT16_MAX));
}

// rand_livelihood - 民生主义, 利益是人民所共享, 让人像个人
static void rand_livelihood(void) {
    for (int32_t i = r_rand(); i >= 0; --i)
        r_rand();
}

// rand_restrict - 三权制衡在随机函数初中运用
void rand_restrict(void) {
    thread_async(rand_nationalism);
    thread_async(rand_democracy);
    thread_async(rand_livelihood);
}
