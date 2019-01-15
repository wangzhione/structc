#ifndef _ATOMIC_H
#define _ATOMIC_H

#include <stddef.h>
#include <stdint.h>

#include "atomic$cl.h"
#include "atomic$c11.h"

/*
 * This header gives more or less a backport of C11 atomics. The user
 * can write GENERIC_ATOMIC(type, tail, size) to generate
 * counterparts of the C11 atomic functions for type, as so:
 *   GENERIC_ATOMIC(int *, pi, PTR_SIZEOF)
 * and then write things like:
 *   atomic_pi_t ptr;
 *   int * some = NULL;
 *   atomic_store_pi(&ptr, some, memory_order_relaxed);
 *   int * prev = atomic_exchange_pi(&ptr, NULL, memory_order_acq_rel);
 *   assert(some == prev);
 * and expect things to work in the obvious way.
 *
 * Also included (with naming differences to avoid conflicts with the
 * standard library):
 *   mimics C11's ATOMIC_VAR_INIT
 *   mimics C11's atomic_thread_fence(memory_order)
 */

// sys/types.h constructor
#ifndef __ssize_t_defined
typedef ptrdiff_t ssize_t;
#endif

#ifndef PTR_SIZEOF
#  if defined(_M_X64) || defined(__x86_64__) || defined(_M_ARM64)
#    define PTR_SIZEOF 8
#  else
#    define PTR_SIZEOF 4
#  endif
#endif

GENERIC_ATOMIC(bool, b, 1)

GENERIC_INT_ATOMIC(unsigned, u, 4)

GENERIC_INT_ATOMIC(uint32_t, u32, 4)

GENERIC_ATOMIC(void *, p, PTR_SIZEOF)

/*
 * Not all platforms have 64-bit atomics. If we do, this #define
 * exposes that fact.
 */
#if (PTR_SIZEOF == 8)
GENERIC_INT_ATOMIC(uint64_t, u64, 8)
#endif

GENERIC_INT_ATOMIC(size_t, zu, PTR_SIZEOF)

GENERIC_INT_ATOMIC(ssize_t, zd, PTR_SIZEOF)

#endif//_ATOMIC_H
