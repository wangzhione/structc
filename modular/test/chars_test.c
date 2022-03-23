#include <chars.h>
#include <spinlock.h>

void chars_test(void) {
    struct chars cs = {};

    chars_appendc(&cs, 'V');

    printf("str = %s, cap = %zu, len = %zu\n", chars_get(&cs), cs.cap, cs.len);

    char * str = chars_sprintf(&cs, "还好吗, %d", 84);
    printf("str = %s, cap = %zu, len = %zu\n", str, cs.cap, cs.len);

    free(cs.str);

    struct cstr {
        struct chars str;
        atomic_flag lock;
    };

    // 初始化
    struct cstr a = { ATOMIC_FLAG_INIT };

    // 加锁
    atomic_flag_lock(&a.lock);

    // 使用
    // 各种对于 astr.str 操作都是线程安全的
    // ...
    chars_appends(&a.str, "100");

    printf("str = %s, cap = %zu, len = %zu\n", chars_get(&a.str), a.str.cap, a.str.len);

    // 释放锁
    atomic_flag_unlock(&a.lock);

    printf("a = 0x%p, a.str = 0x%p, a.str.str = 0x%p\n", &a, &a.str, a.str.str);

    // 销毁
    free(a.str.str);
}
