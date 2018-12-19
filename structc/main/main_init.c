#include "head.h"
#include "socket.h"

//
// STR_CONF_PATH - 配置文件路径
// STR_LOG_PATH  - 日志文件路径
// 
#define STR_CONF_PATH       "conf/conf.conf"
#define STR_LOG_PATH        "logs/structc.log"

//
// main_init - 模块初始化
// return   : void
//
void main_init(void) {
    int n;
    char r[BUFSIZ];

    // 随机数模块初始化 :)
    //
    EXTERN_RUN(r_init);

    // socket 模块初始化 ~
    //
    socket_init();

    // 配置模块初始化
    //
    n = getawd(r, sizeof r);
    assert(0 < n && n < sizeof r);
    strcat(r, STR_CONF_PATH);
    IF(!conf_init(r));

    //  日志模块初始化
    //
    memcpy(r+n, STR_LOG_PATH, LEN(STR_LOG_PATH));
    mkfdir(r);
    EXTERN_RUN(log_init, r);
}
