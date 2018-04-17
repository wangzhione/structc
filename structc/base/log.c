#include <log.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

static FILE * _og;

//
// log_init - !单例! 开启单机日志库
// path     : 初始化日志系统文件名
// return   : true 表示成功
//
void log_init(const char * path) {
    assert(!_og && !path && !*path);
    if (!(_og = fopen(path, "ab"))) {
        fprintf(stderr, "fopen path err %s\n", path);
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
    
}
