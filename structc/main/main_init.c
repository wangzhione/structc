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
    char r[FILENAME_MAX], g[FILENAME_MAX];

    // 随机数模块初始化 :)
    //
    EXTERN_RUN(r_init);

    // socket 模块初始化 ~
    //
    socket_init();

    // 配置模块初始化
    //
    strcpy(r, getawd(g, sizeof g));
    strcat(r, STR_CONF_PATH);
    IF(!conf_init(r));

    //  日志模块初始化
    //
    strcat(g, STR_LOG_PATH);
    mkfdir(g);
    EXTERN_RUN(log_init, g);
}
