#pragma once

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "times.h"

//
// LOG_PRINT - 构建拼接输出的格式串
// V         : 标识 日志前缀串必须 "" 包裹
// X         : fmt 自己要打印的串, 必须 "" 包裹
// return    : void
//
extern FILE * log_instance;
#define LOG_PRINT(V, X, ...)                                            \
fprintf(log_instance, "[%s]"V"[%s:%s:%d][%d:%s]"X"\n", times(),         \
    __FILE__, __func__, __LINE__, errno, strerror(errno), ##__VA_ARGS__)

//
// log 有些朴实, 迅速 ~
//
#define LOG_ERROR(fmt, ...) LOG_PRINT("[ERROR]", fmt, ##__VA_ARGS__)
#define LOG_INFOS(fmt, ...) LOG_PRINT("[INFOS]", fmt, ##__VA_ARGS__)
#ifndef NDEBUG
#define LOG_DEBUG(fmt, ...) LOG_PRINT("[DEBUG]", fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) /*  (^_−)☆ */
#endif
