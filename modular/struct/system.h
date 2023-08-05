﻿#pragma once

//
// 辽阔的夜, 黝黑而深邃 ~ 
// 
// 苏堤春晓
// 明·杨周
// 
// 柳暗花明春正好，重湖雾散分林鸟。
// 何处黄鹂破暝烟，一声啼过苏堤晓。
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

// defined(__linux__) && defined(__GNUC__)
// defined(_WIN32)    && defined(_MSC_VER)
// defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
//

//
// 约定: BEST NEW VERSION 操作系统 Linux + 编译工具 GCC
//
#if defined(__linux__) && defined(__GNUC__)

# if defined(__x86_64__)
#  define ISX64
# endif

//
// 大小端检测 : ISBIG defined 表示大端
//
# if defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN_BITFIELD)
#  define ISBIG
# endif

# ifndef likely
#   define likely(x)   __builtin_expect(!!(x), 1)
# endif
# ifndef unlikely
#   define unlikely(x) __builtin_expect(!!(x), 0)
# endif

#endif

#ifndef UNUSED
#define UNUSED(parameter) if (parameter) {}
#endif//UNUSED

//
// DCODE - DEBUG 模式下的测试宏
// DCODE({
//     puts("debug test start ...");
// });
//
#ifndef DCODE
#  ifndef NDEBUG
#    define DCODE(code)  do code while(0)
#  else
#    define DCODE(code)  
#  endif//NDEBUG
#endif//DCODE

//
// EXTERN_RUN - 函数包装宏, 声明并立即使用
// frun     : 需要执行的函数名称
// ...      : 可变参数, 保留
//
#define EXTERN_RUN(frun, ...)                                           \
do {                                                                    \
    extern void frun();                                                 \
    frun (__VA_ARGS__);                                                 \
} while(0)

// PRINTF fprintf 包装操作宏. time_t x64 8字节 window %lld, linux %ld
#define PRINTF(stream, error, fmt, ...)                                 \
fprintf(stream, "[%"PRId64"]["#stream"][%s:%s:%d][%d:%s]"fmt"\n",       \
    time(NULL),                                                         \
    __FILE__, __func__, __LINE__, error, strerror(error), ##__VA_ARGS__)

#define POUT(fmt, ...)                                                  \
PRINTF(stdout, errno, fmt, ##__VA_ARGS__)

#define PERROR(error, fmt, ...)                                         \
PRINTF(stderr, error, fmt, ##__VA_ARGS__)

//
// PERR - 打印错误信息
// EXIT - 打印错误信息, 并 exit
// IF   - 条件判断异常退出的辅助宏
//

#define PERR(fmt, ...)                                                  \
PRINTF(stderr, errno, fmt, ##__VA_ARGS__)

#define EXIT(fmt, ...)                                                  \
do {                                                                    \
    PERR(fmt, ##__VA_ARGS__);                                           \
    exit(EXIT_FAILURE);                                                 \
} while(0)

#define IF(cond)                                                        \
if ((cond)) EXIT(#cond)

//
// RETURN - 打印错误信息, 并 return 返回指定结果
// val      : return 的东西. 填 NIL 标识 return void;
// fmt      : 双引号包裹的格式化字符串
// ...      : fmt 中对应的参数
// return   : val
// 
#define RETURN(val, fmt, ...)                                           \
do {                                                                    \
    PERR(fmt, ##__VA_ARGS__);                                           \
    return val;                                                         \
} while(0)

#define NIL
#define RETNIL(fmt, ...)                                                \
RETURN(NIL , fmt, ##__VA_ARGS__)

#define RETNUL(fmt, ...)                                                \
RETURN(NULL, fmt, ##__VA_ARGS__)

// -1 是系统开发中最常见也算默认 error value 标识
#define RETERR(fmt, ...)                                                \
RETURN(-1  , fmt, ##__VA_ARGS__)

