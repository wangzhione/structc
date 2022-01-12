#include <cstr.h>
#include <spinlock.h>

void cstr_test(void) {
    struct cstr cs[1] = { { 0 } };

    cstr_appendc(cs, 'V');

    printf("str = %s, cap = %zu, len = %zu\n", cstr_get(cs), cs->cap, cs->len);

    struct astr {
        atomic_flag lock;
        struct cstr str[1];
    };

    // 初始化
    struct astr a = { ATOMIC_FLAG_INIT };

    // 加锁
    atomic_flag_lock(&a.lock);

    // 使用
    // 各种对于 astr.str 操作都是线程安全的
    // ...
    cstr_appends(a.str, "100");

    printf("str = %s, cap = %zu, len = %zu\n", cstr_get(a.str), a.str->cap, a.str->len);

    // 释放锁
    atomic_flag_unlock(&a.lock);

    // 销毁
    cstr_free(a.str);
}