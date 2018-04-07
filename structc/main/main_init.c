#include <chead.h>
#include <socket.h>
#include <config.h>

// STR_CONFIG 全局配置文件路径
#define STR_CONFIG      "config/config.json"

//
// main_init - 各个模块初始化
// return   : void
//
void main_init(void) {
    socket_init();

    EXTERN_RUN(r_init);

    // 初始化配置相关内容
    CERR_IF(!config_init(STR_CONFIG));
}
