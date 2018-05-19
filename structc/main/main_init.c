#include <chead.h>
#include <socket.h>

//
// STR_CONF_PATH - 配置文件路径
// STR_LOG_PATH  - 日志文件路径
// 
#define STR_CONF_PATH       "conf/conf.conf"
#define STR_LOG_PATH        "logs/structc.log"

//
// mkfdir - 通过文件路径创建目录
// path     : 文件路径
// return   : < 0 is error, 0 is success
//
int mkfdir(const char * path) {
    int c;
    char * dir;
    const char * sir;
    if (!path) return EParam;

    for (sir = path; (c = *sir) != '\0'; ++sir)
        if (c == '/' || c == '\\')
            break;
    if (c == '\0') return SBase;

    // 复制地址地址并构建
    dir = strdup(path);
    dir[sir-path] = '\0';
    c = mkdirs(dir);
    free(dir);
    return c;
}

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
