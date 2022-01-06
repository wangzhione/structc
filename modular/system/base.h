#pragma once

#include "log.h"
#include "rand.h"
#include "conf.h"
#include "check.h"
#include "thread.h"
#include "strext.h"

//
// STR - 添加双引号的宏 
// v        : 变量标识
//
#define STR(v)  S_R(v)
#define S_R(v)  #v

#ifndef LEN
//
// LEN - 计算获取数组长度
// a        : 数组变量
//
#define LEN(a)  ((int)(sizeof(a) / sizeof(*(a))))
#endif

//
// CODE_RUN - 代码块测试, 并输出运行时间
// code     : { ... } 包裹的代码块
//
#define CODE_RUN(code)                                 \
do {                                                   \
    clock_t $s = clock();                              \
    code                                               \
    double $e = (double)clock();                       \
    printf("code run %lfs\n", ($e-$s)/CLOCKS_PER_SEC); \
} while (0)

//
// EXTERN_RUN - 函数包装宏, 声明并立即使用
// frun     : 需要执行的函数名称
// ...      : 可变参数, 保留
//
#define EXTERN_RUN(frun, ...)                          \
do {                                                   \
    extern void frun();                                \
    frun (__VA_ARGS__);                                \
} while(0)
