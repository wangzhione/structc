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
    socket_init();

    EXTERN_RUN(r_init);

    // 初始化配置相关内容
    IF(!conf_init(STR_CONF_PATH));

    //
    //  日志模块初始化
    //
    mkfdir(STR_LOG_PATH);
    EXTERN_RUN(log_init, STR_LOG_PATH);
}
