#if !defined(_ATOMIC$C11_H) && !defined(_MSC_VER)
#define _ATOMIC$C11_H

#include <stdbool.h>
#include <stdatomic.h>

#define GENERIC_ATOMIC(type, mark, /* unused */ size)               \
                                                                    \
typedef _Atomic(type) atomic_##mark##_t;                            \
                                                                    \
static inline type                                                  \
atomic_load_##mark(const atomic_##mark##_t * a, memory_order o) {   \
   /*                                                               \
    * A strict interpretation of the C standard prevents            \
    * atomic_load from taking a const argument, but it's            \
    * convenient for our purposes. This cast is a workaround.       \
    */                                                              \
    return atomic_load_explicit((atomic_##mark##_t *)a, o);         \
}                                                                   \
                                                                    \
static inline void                                                  \
atomic_store_##mark(atomic_##mark##_t * a, type v,                  \
                    memory_order o) {                               \
    atomic_store_explicit(a, v, o);                                 \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_exchange_##mark(atomic_##mark##_t * a, type v,               \
                       memory_order o) {                            \
    return atomic_exchange_explicit(a, v, o);                       \
}                                                                   \
                                                                    \
static inline bool                                                  \
atomic_compare_exchange_weak_##mark(atomic_##mark##_t * a,          \
                                    type * c, type v,               \
                                    memory_order o,                 \
                                    memory_order n) {               \
    return atomic_compare_exchange_weak_explicit(a, c, v, o, n);    \
}                                                                   \
                                                                    \
static inline bool                                                  \
atomic_compare_exchange_strong_##mark(atomic_##mark##_t * a,        \
                                      type * c, type v,             \
                                      memory_order o,               \
                                      memory_order n) {             \
    return atomic_compare_exchange_strong_explicit(a, c, v, o, n);  \
}

//
// Integral types have some special operations available that
// non-integral ones lack.
//
#define GENERIC_INT_ATOMIC(type, mark, /* unused */ size)           \
                                                                    \
GENERIC_ATOMIC(type, mark, size)                                    \
                                                                    \
static inline type                                                  \
atomic_fetch_add_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    return atomic_fetch_add_explicit(a, v, o);                      \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_sub_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    return atomic_fetch_sub_explicit(a, v, o);                      \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_and_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    return atomic_fetch_and_explicit(a, v, o);                      \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_or_##mark(atomic_##mark##_t * a, type v,               \
                       memory_order o) {                            \
    return atomic_fetch_or_explicit(a, v, o);                       \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_xor_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    return atomic_fetch_xor_explicit(a, v, o);                      \
}

#endif//_ATOMIC$C11_H
