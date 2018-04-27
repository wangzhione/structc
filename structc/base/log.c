#include <log.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static FILE * _og;

//
// log_init - !单例! 开启单机日志库
// path     : 初始化日志系统文件名
// return   : true 表示成功
//
void log_init(const char * path) {
    if (!(_og = fopen(path, "ab"))) {
        fprintf(stderr, "fopen ab path err %s\n", path);
        exit(EXIT_FAILURE);
    }
}

//
// log_printf - 具体输出日志内容
// fmt      : 必须双引号包裹起来的串
// ...      : 对映fmt参数
// return   : void
//
void 
log_printf(const char * fmt, ...) {
    va_list ap;
    // 每条日志大小, 按照系统缓冲区走
    char str[BUFSIZ];
    int len = times_fmt("[" STR_TIMES "]", str, sizeof str);

    // 日志内容填充
    va_start(ap, fmt);
    vsnprintf(str + len, sizeof str - len, fmt, ap);
    va_end(ap);

    // 数据写入到文件中
    fputs(str, _og);
}
