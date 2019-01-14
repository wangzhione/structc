#include <atom.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

// atom_test - 为 atom.h 写的单元测试
void atom_test(void) {
    atomic_p_t ptr;
    char * some = "Hello, 世界";
    atomic_store_p(&ptr, some, memory_order_relaxed);
    char * prev = atomic_exchange_p(&ptr, NULL, memory_order_acq_rel);
    assert(some == prev);

    printf("%s\n", prev);

    // 自旋锁测试
    atom_t o = 0;
    printf("atom_t o -> %ld\n", o);

    if (atom_trylock(o)) {
        printf("atom_trylock get lock o = %ld\n", o);
        atom_unlock(o);
    } else {
        printf("atom_trylock no get lock o = %ld\n", o);
    }

    printf("atom_t o -> %ld\n", o);

    int id = INT_MAX;
    int now = atom_inc(id);
    now = atom_inc(id);
    printf("id = %d, now = %d\n", id, now);
    if (now < 0)
        now = atom_and(id, INT_MAX);
    printf("id = %d, now = %d\n", id, now);
}