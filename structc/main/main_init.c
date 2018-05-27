#include <chead.h>
#include <socket.h>

//
// STR_CONF_PATH - 配置文件路径
// STR_LOG_PATH  - 日志文件路径
// 
#define STR_CONF_PATH       "conf/conf.conf"
#define STR_LOG_PATH        "logs/structc.log"

//
// main_init - 各个模块初始化
// return   : void
//
void main_init(void) {
    char r[BUFSIZ];
    socket_init();

    EXTERN_RUN(r_init);

    // 初始化配置相关内容
    strcat(getawd(r, sizeof r), STR_CONF_PATH);
    IF(!conf_init(r));

    //
    //  日志模块初始化
    //
    strcat(getawd(r, sizeof r), STR_LOG_PATH);
    mkfdir(r);
    EXTERN_RUN(log_init, r);
}
