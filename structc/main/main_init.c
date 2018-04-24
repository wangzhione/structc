#include <chead.h>
#include <socket.h>
#include <config.h>

//
// STR_CONFIG_PATH - 配置文件路径
// 
#define STR_CONFIG_PATH     "conf/config.conf"
#define STR_LOG_DIR         "logs"
#define STR_LOG_PATH        STR_LOG_DIR "/structc.log"

//
// main_init - 各个模块初始化
// return   : void
//
void main_init(void) {
    socket_init();

    EXTERN_RUN(r_init);

    // 初始化配置相关内容
    IF(!config_init(STR_CONFIG_PATH));

    //
    //  日志模块初始化
    //
    
    EXTERN_RUN(log_init, STR_LOG_PATH);
}
