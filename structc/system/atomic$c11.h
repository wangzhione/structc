#if !defined(_H_ATOMIC$C11) && defined(__GNUC__)
#define _H_ATOMIC$C11

#include <stdatomic.h>

#define ATOMIC_INIT(...) ATOMIC_VAR_INIT(__VA_ARGS__)

#define atomic_memory_order_t memory_order
#define atomic_memory_order_relaxed memory_order_relaxed
#define atomic_memory_order_acquire memory_order_acquire
#define atomic_memory_order_release memory_order_release
#define atomic_memory_order_acq_rel memory_order_acq_rel
#define atomic_memory_order_seq_cst memory_order_seq_cst

#define atomic_fence atomic_thread_fence

#define GENERATE_ATOMICS(type, short_type, /* unused */ lg_size)        \
typedef _Atomic(type) atomic_##short_type##_t;                          \
                                                                        \
static inline type atomic_load_##short_type(                            \
    const atomic_##short_type##_t * a, atomic_memory_order_t mo) {      \
    /*                                                                  \
     * A strict interpretation of the C standard prevents               \
     * atomic_load from taking a const argument, but it's               \
     * convenient for our purposes. This cast is a workaround.          \
     */                                                                 \
    atomic_##short_type##_t * a_noconst = (atomic_##short_type##_t *)a; \
    return atomic_load_explicit(a_noconst, mo);                         \
}                                                                       \
                                                                        \
static inline void atomic_store_##short_type(                           \
    atomic_##short_type##_t * a, type val, atomic_memory_order_t mo) {  \
    atomic_store_explicit(a, val, mo);                                  \
}                                                                       \
                                                                        \
static inline type atomic_exchange_##short_type(                        \
    atomic_##short_type##_t * a, type val, atomic_memory_order_t mo) {  \
    return atomic_exchange_explicit(a, val, mo);                        \
}                                                                       \
                                                                        \
static inline _Bool atomic_compare_exchange_weak_##short_type(          \
    atomic_##short_type##_t * a,                                        \
    type * expected, type desired, atomic_memory_order_t success_mo,    \
    atomic_memory_order_t failure_mo) {                                 \
    return atomic_compare_exchange_weak_explicit(a, expected,           \
        desired, success_mo, failure_mo);                               \
}                                                                       \
                                                                        \
static inline _Bool atomic_compare_exchange_strong_##short_type(        \
    atomic_##short_type##_t * a,                                        \
    type * expected, type desired, atomic_memory_order_t success_mo,    \
    atomic_memory_order_t failure_mo) {                                 \
    return atomic_compare_exchange_strong_explicit(a, expected,         \
        desired, success_mo, failure_mo);                               \
}

/*
 * Integral types have some special operations available that 
 * non-integral ones lack.
 */
#define GENERATE_INT_ATOMICS(type, short_type, /* unused */ lg_size)    \
GENERATE_ATOMICS(type, short_type, /* unused */ lg_size)                \
                                                                        \
static inline type atomic_fetch_add_##short_type(                       \
    atomic_##short_type##_t * a, type val, atomic_memory_order_t mo) {  \
    return atomic_fetch_add_explicit(a, val, mo);                       \
}                                                                       \
                                                                        \
static inline type atomic_fetch_sub_##short_type(                       \
    atomic_##short_type##_t * a, type val, atomic_memory_order_t mo) {  \
    return atomic_fetch_sub_explicit(a, val, mo);                       \
}                                                                       \
                                                                        \
static inline type atomic_fetch_and_##short_type(                       \
    atomic_##short_type##_t * a, type val, atomic_memory_order_t mo) {  \
    return atomic_fetch_and_explicit(a, val, mo);                       \
}                                                                       \
                                                                        \
static inline type atomic_fetch_or_##short_type(                        \
    atomic_##short_type##_t * a, type val, atomic_memory_order_t mo) {  \
    return atomic_fetch_or_explicit(a, val, mo);                        \
}                                                                       \
                                                                        \
static inline type atomic_fetch_xor_##short_type(                       \
    atomic_##short_type##_t * a, type val, atomic_memory_order_t mo) {  \
    return atomic_fetch_xor_explicit(a, val, mo);                       \
}

#endif//_H_ATOMIC$C11
