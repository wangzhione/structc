#ifndef _H_STRUCT
#define _H_STRUCT

#include <math.h>
#include "alloc.h"
#include <ctype.h>
#include <float.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

//
// DCODE - DEBUG 模式下的测试宏
// DCODE({
//     puts("debug start...");
// });
//
#ifndef DCODE
#   ifdef _DEBUG
#       define DCODE(code)  do code while(0)
#   else
#       define DCODE(code)  
#   endif //  ! _DEBUG
#endif  //  ! DCODE

//
// icmp_f - 比较行为的类型
//  : int add_cmp(const void * now, const void * node)
//
typedef int (* icmp_f)();

//
// vnew_f - 根据规则构建对象
//  : void * rtree_new(void * node)
//
typedef void * (* vnew_f)();

//
// node_f - 销毁当前对象节点
//  : void list_die(void * node)
//
typedef void (* node_f)(void * node);

//
// start_f - pthread create func
//  : int * run(int * arg)
//
typedef void * (* start_f)(void * arg);

//
// each_f - each 循环操作, arg 外部参数, node 是内部结点
//  : int echo(void * node, void * arg) { return 0; }
//
typedef int (* each_f)(void * node, void * arg);

//
// CERR - 打印错误信息
// EXIT - 打印错误信息, 并 exit
// IF   - 条件判断异常退出的辅助宏
//
#define CERR(fmt, ...)                                                   \
fprintf(stderr, "[%s:%s:%d][%d:%s]" fmt "\n",                            \
    __FILE__, __func__, __LINE__, errno, strerror(errno), ##__VA_ARGS__)

#define EXIT(fmt, ...)                                                   \
do {                                                                     \
    CERR(fmt, ##__VA_ARGS__);                                            \
    exit(EXIT_FAILURE);                                                  \
} while(0)

#define IF(cond)                                                         \
if ((cond)) EXIT(#cond)

//
// RETURN - 打印错误信息, 并 return 返回指定结果
// val      : return的东西, 当需要 return void; 时候填 ',' 就过 or NIL
// fmt      : 双引号包裹的格式化字符串
// ...      : fmt中对应的参数
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

#endif//_H_STRUCT
