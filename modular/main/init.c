﻿#include "base.h"
#include "socket.h"

//
// CONF_PATH_STR  - 配置文件路径
// LOGS_PATH_STR  - 日志文件路径
// 
#define CONF_PATH_STR               "conf/conf.conf"
#define LOGS_PATH_STR               "logs/structc.log"

//
// init     - 模块初始化
// return   : void
//
void init(void) {
    // Now 'timezome' global is populated. Obtain timezone and daylight info. 
    tzset();

# if defined(_WIN32) && defined(_MSC_VER)
    // 手动设置 window console 编码为 UTF-8
    // 65001    UTF-8 代码页
    // 950      繁体中文
    // 936      简体中文 默认 GBK
    // 437      MS DOS 美国英语
    system("chcp 65001");
# endif

    char path[BUFSIZ];
    // 一切皆有可能 🙂
    size_t n = getawd(path, LEN(path));

    // thread 模块初始化
    EXTERN_RUN(pthread_init);

    // 随机函数 init 😂
    struct timespec s; 
    timespec_get(&s, TIME_UTC);
    EXTERN_RUN(r_init, s.tv_nsec+s.tv_sec);

    // socket 模块初始化 ~
    socket_init();

    // 配置模块初始化
    IF(!conf_init(strcat(path, CONF_PATH_STR)));

    // 日志模块初始化
    memcpy(path + n, LOGS_PATH_STR, LEN(LOGS_PATH_STR));
    fmkdir(path);
    EXTERN_RUN(log_init, path);
}
