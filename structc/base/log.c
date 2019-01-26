#include "log.h"

static FILE * log;

//
// log_init - !单例! 日志库初始化
// path     : 初始化日志系统文件名
// return   : void
//
void log_init(const char * path) {
    if (!(log = fopen(path, "ab"))) {
        fprintf(stderr, "fopen ab path err %s\n", path);
        exit(EXIT_FAILURE);
    }
}

//
// log_printf - 具体输出的日志内容
// fmt      : 必须 "" 包裹的串
// ...      : 对映 fmt 参数
// return   : void
//
void 
log_printf(const char * fmt, ...) {
    va_list ap;
    // 每条日志大小, 按照系统缓冲区走
    char str[BUFSIZ];
    int len = times_fmt("["TIMES_STR"]", str, sizeof str);

    // 填入日志内容
    va_start(ap, fmt);
    vsnprintf(str + len, sizeof str - len, fmt, ap);
    va_end(ap);

    // 数据刷入文件缓存
    fputs(str, log);
}
