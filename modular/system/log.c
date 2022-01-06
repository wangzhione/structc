#include "log.h"

static FILE * txt;

// log_init - 单例, 日志库初始化
void log_init(const char * path) {
    if (!(txt = fopen(path, "ab"))) {
        fprintf(stderr, "fopen ab path err %s\n", path);
        exit(EXIT_FAILURE);
    }
}

//
// log_printf - 日志输出
// fmt      : 必须 "" 包裹的串
// ...      : 对映 fmt 参数
// return   : void
//
void 
log_printf(const char * fmt, ...) {
    va_list ap;
    // 每条日志大小, 按照系统缓冲区走
    char buf[BUFSIZ];
    int n = times_fmt("["TIMES_STR"]", buf, sizeof buf);

    // 填入日志内容
    va_start(ap, fmt);
    vsnprintf(buf + n, sizeof buf - n, fmt, ap);
    va_end(ap);

    // 数据交给文件缓存层
    fputs(buf, txt);
}
