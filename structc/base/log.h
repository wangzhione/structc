#ifndef _LOG_H
#define _LOG_H

#include "times.h"
#include <stdlib.h>
#include <stdarg.h>

//
// LOG_PRINTF - 拼接构建输出的格式串
// pre      : 日志前缀串必须 "" 包裹
// fmt      : 自己要打印的串, 必须 "" 包裹
// return   : void
//
#define LOG_PRINTF(pre, fmt, ...)   \
log_printf(pre"[%s:%s:%d]"fmt"\n", __FILE__, __func__, __LINE__, __VA_ARGS__)

//
// log 有些朴实, 也许很快很安全 ~
//
#define LOG_ERROR(fmt, ...) LOG_PRINTF("[ERROR]", fmt, __VA_ARGS__)
#define LOG_INFOS(fmt, ...) LOG_PRINTF("[INFOS]", fmt, __VA_ARGS__)
#ifdef _DEBUG
#define LOG_DEBUG(fmt, ...) LOG_PRINTF("[DEBUG]", fmt, __VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) /*  (^_−)☆ */
#endif

//
// log_printf - 具体输出的日志内容
// fmt      : 必须 "" 包裹的串
// ...      : 对映 fmt 参数
// return   : void
//
void log_printf(const char * fmt, ...);

#endif//_LOG_H
