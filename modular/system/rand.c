#include "rand.h"

#define X0              (0x330E)
#define X1              (0xABCD)
#define X2              (0x1234)
#define A0              (0xE66D)
#define A1              (0xDEEC)
#define A2              (0x0005)
#define C               (0x000B)

#define N               (16)
#define MASK            ((1 << N) - 1)
#define LOW(x)          ((uint32_t)(x) & MASK)
#define HIGH(x)         LOW((x) >> N)

//
// rand_init - 随机函数对象初始化种子
// r         : 随机函数对象
// seed      : 种子数
// return    : void
//
inline void 
rand_init(rand_t r, int64_t seed) {
    r->x0 = X0; r->x1 = LOW(seed); r->x2 = HIGH(seed);
    r->a0 = A0; r->a1 = A1; r->a2 = A2;
    r->c = C;
}

#define CARRY(x, y)     ((x + y) > MASK) // 基于 16 位判断二者和是否进位
#define ADDRQ(x, y, z)  z = CARRY(x, y); x = LOW(x + y)
#define MUL(m, x, y, z) m = (x) * (y); z##0 = LOW(m); z##1 = HIGH(m)

inline void rand_next(rand_t r) {
    uint32_t m, p0, p1, q0, q1, s0, s1, c0, c1;

    MUL(m, r->a0, r->x0, p);
    ADDRQ(p0, r->c, c0);
    ADDRQ(p1, c0, c1);
    MUL(m, r->a0, r->x1, q);
    ADDRQ(p1, q0, c0);
    MUL(m, r->a1, r->x0, s);

    m = c0 + c1 + CARRY(p1, s0) + q1 + s1 
        + r->a0 * r->x2 + r->a1 * r->x1 + r->a2 * r->x0;
    r->x2 = LOW(m);
    r->x1 = LOW(p1 + s0);
    r->x0 = LOW(p0);
}

//
// rand_get  - 获取一个随机值
// r         : 随机函数对象
// return    : 返回 [0, INT32_MAX] 随机数
//
inline int32_t 
rand_get(rand_t r) {
    rand_next(r);
    return (r->x2 << (N - 1)) + (r->x1 >> 1);
}

//
// 想过 - 朵朵浪花, 也不曾看见云彩飘过 :0
//
static rand_t r_r = { { X0, X1, X2 , A0, A1, A2 , C } };

// EXTERN_RUN(r_init) 启动初始化
extern inline void r_init(int64_t seed) {
    rand_init(r_r, seed);
}

inline int32_t 
r_rand(void) {
    return rand_get(r_r);
}

inline int64_t 
r_ranb(void) {
    uint64_t x = ((r_rand() << N) ^ r_rand()) & INT32_MAX;
    uint64_t y = ((r_rand() << N) ^ r_rand()) & INT32_MAX;
    return ((x << 2 * N) | y) & INT64_MAX;
}
