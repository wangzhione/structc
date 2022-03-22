#pragma once

#include "log.h"
#include "rand.h"
#include "check.h"
#include "thread.h"
#include "strext.h"

//
// STR - 添加双引号的宏 
// v        : 变量标识
//
#ifndef STR
#define STR(v)  S_R(v)
#define S_R(v)  #v
#endif

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
#define EXTERN_RUN(frun, ...)                          \
do {                                                   \
    extern void frun();                                \
    frun (__VA_ARGS__);                                \
} while(0)
