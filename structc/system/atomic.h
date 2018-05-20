#ifndef _H_ATOMIC
#define _H_ATOMIC

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "atomic$cl.h"
#include "atomic$c11.h"

// sys/types.h constructor
#ifndef __ssize_t_defined
typedef ptrdiff_t ssize_t;
#endif

#ifndef LG_SIZEOF_PTR
#   if defined(_M_X64) || defined(__x86_64__) || defined(_M_ARM64)
#       define LG_SIZEOF_PTR 3
#   else
#       define LG_SIZEOF_PTR 2
#   endif
#endif

/*
 * This header gives more or less a backport of C11 atomics. The user can 
 * write GENERATE_ATOMICS(type, short_type, lg_sizeof_type); to generate
 * counterparts of the C11 atomic functions for type, as so:
 *   GENERATE_ATOMICS(int *, pi, 3);
 * and then write things like:
 *   int * some_ptr;
 *   atomic_p_t atomic_ptr_to_int;
 *   atomic_store_p(&atomic_ptr_to_int, some_ptr, ATOMIC_RELAXED);
 *   int * prev_value = atomic_exchange_p(&ptr_to_int, NULL, ATOMIC_ACQ_REL);
 *   assert(some_ptr == prev_value);
 * and expect things to work in the obvious way.
 *
 * Also included (with naming differences to avoid conflicts with the 
 * standard library):
 *   atomic_fence(atomic_memory_order_t) (mimics C11's atomic_thread_fence).
 *   ATOMIC_INIT (mimics C11's ATOMIC_VAR_INIT).
 */

/*
 * Pure convenience, so that we don't have to type "atomic_memory_order_"
 * quite so often.
 *
 * ATOMIC_RELAXED - 宽松操作：没有同步或顺序制约，仅对此操作要求原子性
 *
 * ATOMIC_ACQUIRE - 有此内存顺序的加载操作，在其影响的内存位置进行获得操作：
 *                  当前线程中读或写不能被重排到此加载前。
 *                  其他释放同一原子变量的线程的所有写入，能为当前线程所见
 *
 * ATOMIC_RELEASE - 有此内存顺序的存储操作进行释放操作：当前进程中的读或写不能被重排到此存储后。
 *                  当前线程的所有写入，可见于获得该同一原子变量的其他线程释放获得顺序），
 *                  并且对该原子变量的带依赖写入变得对于其他消费同一原子对象的线程可见
 *
 * ATOMIC_ACQ_REL - 带此内存顺序的读修改写操作既是获得操作又是释放操作。
 *                  当前线程的读或写内存不能被重排到此存储前或后。
 *                  所有释放同一原子变量的线程的写入可见于修改之前，
 *                  而且修改可见于其他获得同一原子变量的线程。 
 *
 * ATOMIC_SEQ_CST - 有此内存顺序的加载操作进行获得操作，存储操作进行释放操作，
 *                  而读修改写操作进行获得操作和释放操作，再加上存在一个单独全序，
 *                  其中所有线程以同一顺序观测到所有修改
 */
#define ATOMIC_RELAXED atomic_memory_order_relaxed
#define ATOMIC_ACQUIRE atomic_memory_order_acquire
#define ATOMIC_RELEASE atomic_memory_order_release
#define ATOMIC_ACQ_REL atomic_memory_order_acq_rel
#define ATOMIC_SEQ_CST atomic_memory_order_seq_cst

/*
 * Not all platforms have 64-bit atomics.  If we do, this #define exposes 
 * that fact.
 */
#if (LG_SIZEOF_PTR == 3)
GENERATE_INT_ATOMICS(uint64_t, u64, 3)
#endif

GENERATE_ATOMICS(void *, p, LG_SIZEOF_PTR)

/*
 * There's no actual guarantee that sizeof(bool) == 1, but it's true on the only
 * platform that actually needs to know the size, MSVC.
 */
GENERATE_ATOMICS(bool, b, 0)

GENERATE_INT_ATOMICS(unsigned, u, 2)

GENERATE_INT_ATOMICS(size_t, zu, LG_SIZEOF_PTR)

GENERATE_INT_ATOMICS(ssize_t, zd, LG_SIZEOF_PTR)

GENERATE_INT_ATOMICS(uint32_t, u32, 2)

#endif//_H_ATOMIC
