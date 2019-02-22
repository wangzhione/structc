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
    // :) 随机数模块初始化
    EXTERN_RUN(r_init);

    // socket 模块初始化 ~
    socket_init();

    // 配置模块初始化
    char r[BUFSIZ];
    int n = getawd(r, sizeof r);
    assert(0 < n && n < sizeof r);
    IF(!conf_init(strcat(r, CONF_PATH_STR)));

    // 日志模块初始化
    memcpy(r+n, LOG_PATH_STR, LEN(LOG_PATH_STR));
    mkfdir(r);
    EXTERN_RUN(log_init, r);
}
