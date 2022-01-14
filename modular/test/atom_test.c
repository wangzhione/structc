#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <spinlock.h>
#include <struct.h>

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
    printf("1 id = %d, now = %d\n", id, atomic_load(&id));
    int now = atomic_fetch_add(&id, 1);
    printf("2 id = %d, now = %d\n", id, now);
    if (now < 0) {
        now = atomic_fetch_and(&id, INT_MAX);
        printf("3 id = %d, now = %d\n", id, now);
    }
    now = atomic_fetch_add(&id, 1);
    printf("4 id = %d, now = %d\n", id, now);
    if (now < 0) {
        now = atomic_fetch_and(&id, INT_MAX);
        printf("5 id = %d, now = %d\n", id, now);
    }
}

static atomic_int A = 0; // 0 - INT_MAX

static int getA() {
    int id = atomic_fetch_add(&A, 1);
    if (id < 0) {
        printf("id = %d, A = %d\n", id, A);
        id = atomic_fetch_and(&A, INT_MAX);
        printf("id = %d, A = %d\n", id, A);
    }
    return id;
}

static atomic_int B = 1; // 1 - INT_MAX

static int getB() {
    int id = atomic_fetch_add(&B, 1);
    if (id < 0) {
        printf("id = %d, B = %d\n", id, B);
        id = atomic_fetch_and(&B, INT_MAX);
        printf("id = %d, B = %d\n", id, B);
    }
    return id;
}

void atomic_int_test(void) {
    int id;
    long i;

    // INT_MIN -> 0 -> INT_MAX -> INT_MIN
    // for (i = INT_MIN; i <= INT_MAX; ++i) {
    //     id = getA();
    //     if (id < 0) {
    //         // error id = -2147483647, A = 1
    //         printf("error id = %d, A = %d\n", id, A);
    //     }
    // }
    id = getA();
    printf("1 id = %d, A = %d, B = %d\n", id, A, B);

    for (i = INT_MIN; i <= INT_MAX; ++i) {
        id = getB();
        if (id < 0) {
            // error id = -2147483647, A = 1
            printf("error id = %d, B = %d\n", id, B);
        }
    }

    printf("2 id = %d, A = %d, B = %d\n", id, A, B);
}

void atomic_add_test(void) {
    int id1, id2;
    atomic_int a = INT_MIN;

    id1 = atomic_fetch_add(&a, 1);
    POUT("id1 = %d, a = %d", id1, a);
    id2 = atomic_fetch_add(&a, 1);
    POUT("id2 = %d, a = %d", id2, a);
    if (id1 < 0) {
        POUT("id1 = %d, a = %d", id1, a);
        id1 = atomic_fetch_and(&a, INT_MAX);
        POUT("id1 = %d, a = %d", id1, a);
    }
    if (id2 < 0) {
        POUT("id2 = %d, a = %d", id2, a);
        id2 = atomic_fetch_and(&a, INT_MAX);
        POUT("id2 = %d, a = %d", id2, a);
    }
}

static _Atomic(uint32_t) C;

int64_t getC() {
    int64_t id = atomic_fetch_add(&C, 1);
    return id;
}

int32_t getV(int32_t id) {
    return (int32_t)((uint32_t)id & INT32_MAX);
}

void atomic_test_C_D() {
    int64_t id1, id2 = 0;
    long i;

    POUT("INT_MIN = %d, -1 = %d, 0 = %d, 1 = %d, INT_MAX = %d", 
        getV(INT_MIN), getV(-1), getV(0), getV(1), getV(INT_MAX));

    // for (i = INT_MIN; i <= INT_MAX; ++i) {
    //     id1 = getC();
    //     id2 = getC();
    //     if (id1 < 0 || id2 < 0 || id1 == id2 || id1 == 0 || id1 == 1 || id1 == INT_MAX) {
    //         POUT("i = %ld, id1 = %ld, id2 = %ld, C = %u", i, id1, id2, C);
    //     }
    // }

    for (i = INT_MIN; i <= INT_MAX; ++i) {
        id1 = getC();
        if (id1 < 0 || id1 == 0 || id1 == 1 || id1 == INT_MAX || id1 == UINT_MAX) {
            POUT("i = %ld, id1 = %ld, C = %u", i, id1, C);
        }
    }

    POUT("i = %ld, id1 = %ld, id2 = %ld, C = %u", i, id1, id2, C);
}

static atomic_int D = 1;

static int getD(void) {
    // 0 -> INT_MAX -> INT_MIN -> 0
    int id = atomic_fetch_add(&D, 1) + 1;
    if (id < 0) {
        POUT("id = %d, D = %d", id, D);
        atomic_fetch_add(&D, 1);
        POUT("id = %d, D = %d", id, D);
        id = atomic_fetch_and(&D, INT_MAX) & INT_MAX;
        POUT("id = %d, D = %d", id, D);
    }
    return id;
}

void atomic_test_D(void) {
    int id;
    long i;

    for (i = INT_MIN; i <= INT_MAX; ++i) {
        id = getD();
        if (id <= 0) {
            PERR("i = %ld, id = %d, D = %d", i, id, D);
        }
    }

    POUT("i = %ld, id = %d, D = %d", i, id, D);
}
