#pragma once

#include <math.h>
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
#include "spinlock.h"

#ifndef CMP_F
#define CMP_F

//
// cmp_f - 比较行为 > 0 or = 0  or < 0
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
// each_f - 遍历行为, node 是内部结点, arg 是外部参数
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
// CERR - 打印错误信息
// EXIT - 打印错误信息, 并 exit
// IF   - 条件判断异常退出的辅助宏
//
#define CERR(fmt, ...)                                                  \
fprintf(stderr, "[%s:%s:%d][%d:%s]"fmt"\n",                             \
    __FILE__, __func__, __LINE__, errno, strerror(errno), ##__VA_ARGS__)

#define EXIT(fmt, ...)                                                  \
do {                                                                    \
    CERR(fmt, ##__VA_ARGS__);                                           \
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
    CERR(fmt, ##__VA_ARGS__);                                           \
    return val;                                                         \
} while(0)

#define NIL
#define RETNIL(fmt, ...)                                                \
RETURN(NIL , fmt, ##__VA_ARGS__)

#define RETNUL(fmt, ...)                                                \
RETURN(NULL, fmt, ##__VA_ARGS__)

#define RETERR(fmt, ...)                                                \
RETURN(-1  , fmt, ##__VA_ARGS__)
