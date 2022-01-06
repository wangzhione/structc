#pragma once

#include <stdint.h>
#include <assert.h>

//
// 传承(抄袭)不灭(创新) rand 库
// 大体思路 
//  { r | r[n+1] = (a*r[n] + c) mod m
//      , n >= 0
//      , m = 0xFFFFFFFFFFFF = 2 ^ 48
//      , a = 0x0005DEECE66D = 0x5DEECE66D,
//      , c = 0x000B         = 0xB
//  }
//
struct rand {
    uint32_t x0, x1, x2;
    uint32_t a0, a1, a2;
    uint32_t c;
};

typedef struct rand rand_t[1];

//
// rand_init - 随机函数对象初始化种子
// r         : 随机函数对象
// seed      : 种子数
// return    : void
//
extern void rand_init(rand_t r, int64_t seed);

//
// rand_get  - 获取一个随机值
// r         : 随机函数对象
// return    : 返回 [0, INT32_MAX] 随机数
//
extern int32_t rand_get(rand_t r);

//
// r_rand - 得到 [0, INT32_MAX] 随机数
// r_ranb - 得到 [0, INT64_MAX] (int64 = big int32) 随机数
// r_rang - 得到 range [min, max] 随机数
//
extern int32_t r_rand(void);

extern int64_t r_ranb(void);

inline int32_t r_rang(int32_t min, int32_t max) {
    assert(max >= min);
    return r_rand() % (max - min + 1) + min;
}
