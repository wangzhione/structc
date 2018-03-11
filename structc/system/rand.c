#include <rand.h>
#include <time.h>
#include <assert.h>

#define CARRY(x, y)     ((x + y) > MASK) // 二者和是否进位, 基于 16位
#define ADDRQ(x, y, z)  (z = CARRY(x, y), x = LOW(x + y))

#define MUL(x, y, z)    l = (x) * (y); z[0] = LOW(l); z[1] = HIGH(l)

static void _rand_next(rand_t r) {
    uint32_t l;
    uint32_t p[2], q[2], s[2], c[2];

    MUL(r->a[0], r->x[0], p);
    ADDRQ(p[0], r->c, c[0]);
    ADDRQ(p[1], c[0], c[1]);
    MUL(r->a[0], r->x[1], q);
    ADDRQ(p[1], q[0], c[0]);
    MUL(r->a[1], r->x[0], s);

    l = c[0] + c[1] + CARRY(p[1], s[0]) + q[1] + s[1]
        + r->a[0] * r->x[2] + r->a[1] * r->x[1] + r->a[2] * r->x[0];
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
    _rand_next(r);
    return (r->x[2] << (N - 1)) + (r->x[1] >> 1);
}

//
// 我 - 想赢, 因为不甘心
//
static rand_t _r = { { { X0, X1, X2 }, { A0, A1, A2 }, C } };

// EXTERN_RUN(r_init) 启动初始化
extern inline void r_init(void) {
    rand_init(_r, time(NULL));
}

//
// r_rand  - 得到 [0, INT32_MAX] 随机数
// r_randk - 得到一个 64 位的 key
// r_rands - 得到 [min, max] 范围内随机数
//
inline int32_t 
r_rand(void) {
    return rand_rand(_r);
}

inline int64_t 
r_randk(void) {
    uint64_t x = ((r_rand() << N) ^ r_rand()) & INT32_MAX;
    uint64_t y = ((r_rand() << N) ^ r_rand()) & INT32_MAX;
    return ((x << 2 * N) | y) & INT64_MAX;
}

inline int32_t 
r_rands(int32_t min, int32_t max) {
    assert(max > min);
    return r_rand() % (max - min + 1) + min;
}
