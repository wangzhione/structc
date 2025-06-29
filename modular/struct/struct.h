#pragma once

#include <math.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <inttypes.h>

//
// STR - 添加双引号的宏 
// v        : 变量标识
//
#ifndef STR
#define STR_(v) #v
#define STR(v)  STR_(v)
#endif

#ifndef CONCAT
#define CONCAT_(X, Y) X ## Y
#define CONCAT(X, Y) CONCAT_(X, Y)
#endif//CONCAT

#ifndef LEN
//
// LEN - 计算获取数组长度
// a        : 数组变量
//
#define LEN(a)  ((int)(sizeof(a) / sizeof(*(a))))
#endif

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
    extern void frun(...);                                              \
    frun (__VA_ARGS__);                                                 \
} while(0)

// PRINTF fprintf 包装操作宏. time_t x64 8字节 window %lld, linux %ld
#define PRINTF(stream, error, fmt, ...)                                 \
fprintf(stream, "[%"PRId64"]["#stream"][%s:%s:%d][%d:%s]"fmt"\n",       \
    time(nullptr),                                                      \
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
RETURN(nullptr, fmt, ##__VA_ARGS__)

// -1 是系统开发中最常见也算默认 error value 标识
#define RETERR(fmt, ...)                                                \
RETURN(-1  , fmt, ##__VA_ARGS__)

//
// 辽阔的夜, 黝黑而深邃 ~ 
// 
// 苏堤春晓
// 明·杨周
// 
// 柳暗花明春正好，重湖雾散分林鸟。
// 何处黄鹂破暝烟，一声啼过苏堤晓。
//

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
// With the new <stdbit.h>
// Endian macros (__STDC_ENDIAN_BIG__, __STDC_ENDIAN_LITTLE__, __STDC_ENDIAN_NATIVE__)

//
// 大小端检测 : ISBIG defined 表示大端
//
# if defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN_BITFIELD)
#  define ISBIG
# endif

#endif

// 消除编译器警告
#ifndef UNUSED
#define UNUSED(parameter) if (parameter) {}
#endif//UNUSED

#ifndef STRUCT_F
#define STRUCT_F

//
// cmp_f - left now node 比较 right input node 行为 > 0 or = 0  or < 0
// 可以类似 int add_cmp(const void * now, const void * node)
//
typedef int (* cmp_f)(...);

//
// new_f - 构建行为
// 可以类似 void * rtree_new(void * node)
//
typedef void * (* new_f)(...);

//
// node_f - 销毁行为
// 可以类似 void list_die(void * node)
//
typedef void (* node_f)(...);

//
// each_f - 遍历行为, node 是内部结点, arg 是外部参数; 返回值推荐 0 标识正确, -1 标识错误
// 可以类似 int echo(void * node, void * arg) { return 0; }
//
typedef int (* each_f)(void * node, void * arg);

#endif//STRUCT_F
