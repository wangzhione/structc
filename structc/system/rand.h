#ifndef _RAND_H
#define _RAND_H

//
// 线程安全的 rand 库, by redis
//

#include <time.h>
#include <stdint.h>
#include <assert.h>

struct rand {
    uint32_t x[3];
    uint32_t a[3];
    uint32_t c;
};

typedef struct rand rand_t[1];

#define X0              (0x330E)
#define X1              (0xABCD)
#define X2              (0x1234)
#define A0              (0xE66D)
#define A1              (0xDEEC)
#define A2              (0x0005)
#define C               (0x000B)

#define N               (16)
#define MASK            ((1 << N) - 1)
#define LOW(x)          ((unsigned)(x) & MASK)
#define HIGH(x)         LOW((x) >> N)

//
// rand_init - 随机函数初始化种子方法
// r        : 随机函数对象
// seed     : 种子数
// return   : void
//
inline void rand_init(rand_t r, int64_t seed) {
    r->x[0] = X0; r->x[1] = LOW(seed); r->x[2] = HIGH(seed);
    r->a[0] = A0; r->a[1] = A1; r->a[2] = A2;
    r->c = C;
}

//
// rand_rand - 获取一个随机值
// r        : 随机函数对象
// return   : 返回 [0, INT32_MAX] 随机数
//
extern int32_t rand_rand(rand_t r);

//
// r_rand  - 得到 [0, INT32_MAX] 随机数
// r_randk - 得到一个 64 位的 key
// r_rands - 得到 [min, max] 范围内随机数
//
extern int32_t r_rand(void);

inline int64_t r_randk(void) {
    uint64_t x = ((r_rand() << N) ^ r_rand()) & INT32_MAX;
    uint64_t y = ((r_rand() << N) ^ r_rand()) & INT32_MAX;
    return ((x << 2 * N) | y) & INT64_MAX;
}

inline int32_t r_rands(int32_t min, int32_t max) {
    assert(max > min);
    return r_rand() % (max - min + 1) + min;
}

#endif//_RAND_H
