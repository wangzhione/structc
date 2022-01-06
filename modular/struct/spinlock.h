#pragma once

#include <stdbool.h>
#include <stdatomic.h>

/*
    https://zh.cppreference.com/w/c/atomic

    https://en.cppreference.com/w/c/atomic/atomic_compare_exchange

    _Bool atomic_compare_exchange_strong(volatile A * obj,
                                            C * expected, C desired);

    _Bool atomic_compare_exchange_weak( volatile A * obj,
                                        C * expected, C desired );

    if (memcmp(obj, expected, sizeof *obj) == 0) {
        memcpy(obj, &desired, sizeof *obj);
        return true;
    } else {
        memcpy(expected, obj, sizeof *obj);
        return false;
    }
*/

inline bool atomic_bool_compare_exchange_weak(volatile atomic_bool * obj) {
    bool expected = false;
    return atomic_compare_exchange_weak(obj, &expected, true);
}

inline bool atomic_bool_compare_exchange_strong(volatile atomic_bool * obj) {
    bool expected = false;
    return atomic_compare_exchange_strong(obj, &expected, true);
}

// spin lock

inline void atomic_flag_lock(volatile atomic_flag * lock) {
    while (atomic_flag_test_and_set(lock)) {}
}

inline void atomic_flag_unlock(volatile atomic_flag * lock) {
    atomic_flag_clear(lock);
}

inline bool atomic_flag_trylock(volatile atomic_flag * lock) {
    return atomic_flag_test_and_set(lock) == 0;
}
