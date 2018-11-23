#include "rand.h"

#define CARRY(x, y)     ((x + y) > MASK) // 基于 16位判断二者和是否进位
#define ADDRQ(x, y, z)  (z = CARRY(x, y), x = LOW(x + y))

#define MUL(l, x, y, z) l = (x) * (y); z[0] = LOW(l); z[1] = HIGH(l)

inline void rand_next(rand_t r) {
    uint32_t l, p[2], q[2], s[2], c[2];

    MUL(l, r->a[0], r->x[0], p);
    ADDRQ(p[0], r->c, c[0]);
    ADDRQ(p[1], c[0], c[1]);
    MUL(l, r->a[0], r->x[1], q);
    ADDRQ(p[1], q[0], c[0]);
    MUL(l, r->a[1], r->x[0], s);

    l = c[0] + c[1] + CARRY(p[1], s[0]) + q[1] + s[1] + 
        r->a[0] * r->x[2] + r->a[1] * r->x[1] + r->a[2] * r->x[0];
    r->x[2] = LOW(l);
    r->x[1] = LOW(p[1] + s[0]);
    r->x[0] = LOW(p[0]);
}

//
// rand_rand - 获取一个随机值
// r        : 随机函数对象
// return   : 返回 [0, INT32_MAX] 随机数
//
inline int32_t 
rand_rand(rand_t r) {
    rand_next(r);
    return (r->x[2] << (N - 1)) + (r->x[1] >> 1);
}

//
// 我 - 想云, 因为不甘心 :0
//
static rand_t r_r = { { { X0, X1, X2 }, { A0, A1, A2 }, C } };

// EXTERN_RUN(r_init) 启动初始化
extern inline void r_init(void) {
    rand_init(r_r, time(NULL));
}

//
// r_rand  - 得到 [0, INT32_MAX] 随机数
// r_randk - 得到一个 64 位的 key
// r_rands - 得到 [min, max] 范围内随机数
//
inline int32_t 
r_rand(void) {
    return rand_rand(r_r);
}
