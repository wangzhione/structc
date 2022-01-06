#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <spinlock.h>

// atom_test - 为 atom.h 写的单元测试
void atom_test(void) {
    // _Atomic(char *) ptr 不等价 _Atomic char * ptr
    _Atomic(char *) ptr;
    char * some = "Hello, 世界";
    atomic_store_explicit(&ptr, some, memory_order_relaxed);
    char * prev = atomic_exchange_explicit(&ptr, NULL, memory_order_acq_rel);
    assert(some == prev);

    printf("%s\n", prev);

    // 自旋锁测试
    atomic_flag o = ATOMIC_FLAG_INIT;
    printf("atomic_flag start \n");

    if (atomic_flag_trylock(&o)) {
        printf("atomic_flag_trylock get lock\n");
        atomic_flag_unlock(&o);
    } else {
        printf("atomic_flag_trylock no get lock\n");
    }

    atomic_int id = INT_MAX;
    int now = atomic_fetch_add(&id, 1);
    now = atomic_fetch_add(&id, 1);
    printf("id = %d, now = %d\n", id, now);
    if (now < 0)
        now = atomic_fetch_and(&id, INT_MAX);
    printf("id = %d, now = %d\n", id, now);
}