#include <chead.h>
#include <socket.h>
#include <config.h>

//
// main_init - 各个模块初始化
// return   : void
//
void main_init(void) {
    socket_init();

    EXTERN_RUN(r_init);

    // 初始化配置相关内容
    CERR_IF(!config_init());
}
