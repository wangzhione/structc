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

    struct astr a = { ATOMIC_FLAG_INIT };

    atomic_flag_lock(&a.lock);

    cstr_appends(a.str, "100");

    printf("str = %s, cap = %zu, len = %zu\n", cstr_get(a.str), a.str->cap, a.str->len);

    atomic_flag_unlock(&a.lock);

    cstr_free(a.str);
}