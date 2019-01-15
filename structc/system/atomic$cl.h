#if !defined(_ATOMIC$CL_H) && defined(_MSC_VER)
#define _ATOMIC$CL_H

#include <stdbool.h>
#include <windows.h>

typedef enum memory_order {
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst,
} memory_order;

#define ATOMIC_VAR_INIT(...) {__VA_ARGS__}

static inline void atomic_thread_fence(memory_order o) {
    _ReadWriteBarrier();
#  if defined(_M_ARM) || defined(_M_ARM64)
    // ARM needs a barrier for everything but relaxed.
    if (o != memory_order_relaxed)
        MemoryBarrier();
#  elif defined(_M_IX86) || defined (_M_X64)
    // x86 needs a barrier only for seq_cst.
    if (o == memory_order_seq_cst)
        MemoryBarrier();
#  else
#    error "Don't know how to create atomics for this platform for cl."
#  endif
    _ReadWriteBarrier();
}

#define INTERLOCKED_SUFFIX_1 8
#define INTERLOCKED_SUFFIX_2 16
#define INTERLOCKED_SUFFIX_4
#define INTERLOCKED_SUFFIX_8 64

#define CONCAT(a, b)                 CONCAT_RAW(a, b)
#define CONCAT_RAW(a, b)             a##b

typedef char    atomic_size_1_t;
typedef short   atomic_size_2_t;
typedef long    atomic_size_4_t;
typedef __int64 atomic_size_8_t;

#define INTERLOCKED_TYPE(size)       atomic_size_##size##_t

#define INTERLOCKED_SUFFIX(size)     CONCAT(INTERLOCKED_SUFFIX_, size)

#define INTERLOCKED_NAME(name, size) CONCAT(name, INTERLOCKED_SUFFIX(size))

#define GENERIC_ATOMIC(type, mark, size)                            \
                                                                    \
typedef struct {                                                    \
    INTERLOCKED_TYPE(size) repr;                                    \
} atomic_##mark##_t;                                                \
                                                                    \
static inline type                                                  \
atomic_load_##mark(const atomic_##mark##_t * a, memory_order o) {   \
    INTERLOCKED_TYPE(size) ret = a->repr;                           \
    if (o != memory_order_relaxed)                                  \
        atomic_thread_fence(memory_order_acquire);                  \
    return (type)ret;                                               \
}                                                                   \
                                                                    \
static inline void                                                  \
atomic_store_##mark(atomic_##mark##_t * a, type v,                  \
                    memory_order o) {                               \
    if (o != memory_order_relaxed)                                  \
        atomic_thread_fence(memory_order_release);                  \
    a->repr = (INTERLOCKED_TYPE(size))v;                            \
    if (o == memory_order_seq_cst)                                  \
        atomic_thread_fence(memory_order_seq_cst);                  \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_exchange_##mark(atomic_##mark##_t * a, type v,               \
                       memory_order o) {                            \
    return (type)INTERLOCKED_NAME(_InterlockedExchange, size)(      \
                &a->repr, (INTERLOCKED_TYPE(size))v                 \
           );                                                       \
}                                                                   \
                                                                    \
static inline bool                                                  \
atomic_compare_exchange_weak_##mark(atomic_##mark##_t * a,          \
                                    type * c, type v,               \
                                    memory_order o,                 \
                                    memory_order n) {               \
    INTERLOCKED_TYPE(size) d = (INTERLOCKED_TYPE(size))v;           \
    INTERLOCKED_TYPE(size) e = (INTERLOCKED_TYPE(size))*c;          \
    INTERLOCKED_TYPE(size) old =                                    \
        INTERLOCKED_NAME(_InterlockedCompareExchange, size)(        \
            &a->repr, d, e                                          \
        );                                                          \
    if (e != old) {                                                 \
        *c = (type)old;                                             \
        return false;                                               \
    }                                                               \
    return true;                                                    \
}                                                                   \
                                                                    \
static inline bool                                                  \
atomic_compare_exchange_strong_##mark(atomic_##mark##_t * a,        \
                                      type * c, type v,             \
                                      memory_order o,               \
                                      memory_order n) {             \
    /* We implement the weak version with strong semantics. */      \
    return atomic_compare_exchange_weak_##mark(a, c, v, o, n);      \
}

#define GENERIC_INT_ATOMIC(type, mark, size)                        \
                                                                    \
GENERIC_ATOMIC(type, mark, size)                                    \
                                                                    \
static inline type                                                  \
atomic_fetch_add_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    return (type)INTERLOCKED_NAME(_InterlockedExchangeAdd, size)(   \
                &a->repr, (INTERLOCKED_TYPE(size))v                 \
           );                                                       \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_sub_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    /*                                                              \
     * MSVC warns on negation of unsigned operands, but for us it   \
     * gives exactly the right semantics (MAX_TYPE + 1 - operand).  \
     */                                                             \
    __pragma(warning(push))                                         \
    __pragma(warning(disable: 4146))                                \
    return atomic_fetch_add_##mark(a, -v, o);                       \
    __pragma(warning(pop))                                          \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_and_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    return (type)INTERLOCKED_NAME(_InterlockedAnd, size)(           \
                &a->repr, (INTERLOCKED_TYPE(size))v                 \
           );                                                       \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_or_##mark(atomic_##mark##_t * a, type v,               \
                       memory_order o) {                            \
    return (type)INTERLOCKED_NAME(_InterlockedOr, size)(            \
                &a->repr, (INTERLOCKED_TYPE(size))v                 \
           );                                                       \
}                                                                   \
                                                                    \
static inline type                                                  \
atomic_fetch_xor_##mark(atomic_##mark##_t * a, type v,              \
                        memory_order o) {                           \
    return (type)INTERLOCKED_NAME(_InterlockedXor, size)(           \
                &a->repr, (INTERLOCKED_TYPE(size))v                 \
           );                                                       \
}

#endif//_ATOMIC$CL_H
