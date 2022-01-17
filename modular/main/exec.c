#include "base.h"

#include "conf/conf.h"

//
// CONF_PATH_STR  - 配置文件路径
// LOGS_PATH_STR  - 日志文件路径
// 
#define CONF_PATH_STR               "conf/conf.conf"
#define LOGS_PATH_STR               "logs/structc.log"

// exec 业务之前 init
void exec_init(void) {
    char path[BUFSIZ];
    // 一切皆有可能 🙂
    size_t n = getawd(path, LEN(path));

    // 配置模块初始化
    IF(!conf_init(strcat(path, CONF_PATH_STR)));

    // 日志模块初始化
    memcpy(path + n, LOGS_PATH_STR, LEN(LOGS_PATH_STR));
    fmkdir(path);
    EXTERN_RUN(log_init, path);
}

//
// exec     - 执行业务 🐖🐍🐉
// return   : void
//
void exec(void) {
    //
    // .... .. . 开始你的表演
    //
    struct conf * conf = conf_instance();

    fprintf(stderr, "I am here\n");

    puts(conf->description);
    puts(conf->image);
}
