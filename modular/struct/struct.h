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
#include <stdbool.h>
#include <inttypes.h>

#include "alloc.h"

#ifndef CMP_F
#define CMP_F

//
// cmp_f - left now node 比较 right input node 行为 > 0 or = 0  or < 0
// : int add_cmp(const void * now, const void * node)
//
typedef int (* cmp_f)();

#endif//CMP_F

#ifndef NEW_F
#define NEW_F

//
// new_f - 构建行为
// : void * rtree_new(void * node)
//
typedef void * (* new_f)();

#endif//NEW_F

#ifndef NODE_F
#define NODE_F

//
// node_f - 销毁行为
// : void list_die(void * node)
//
typedef void (* node_f)();

#endif//NODE_F

#ifndef EACH_F
#define EACH_F

//
// each_f - 遍历行为, node 是内部结点, arg 是外部参数; 返回值推荐 0 标识正确, -1 标识错误
// : int echo(void * node, void * arg) { return 0; }
//
typedef int (* each_f)(void * node, void * arg);

#endif//EACH_F

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
