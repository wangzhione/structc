#include "struct.h"
#include "spinlock.h"

//
// 读写锁被用来保护经常需要读, 但是不频繁修改修改的数据.
// 特性是: 可重复读, 互斥写.
// 传统 pthread 提供读写锁, 读和写是交叉的, 一段时间内读多那么读获取锁很容易, 
// 同样写多的时候写获取锁很容易. 因而在读操作频繁时候, 容易造成写饥渴.
// 
// 我们这里实现是, 读锁会压制写锁. 优势是少量写场景写锁不会被饥渴.
// 实现方面采用原子自旋方式, 适用于业务短小快速高频领域.  
//
// 没有一种方案会通杀所有场景, 具体需要因地制宜.  
// atomic, pthread mutex, pthread cond, 业务 之间取舍侧重. 
// 最好方案还是通过业务来避免锁. 万不得已最稳方案还是成熟 pthread 提供相关 api 能力. 
// 
// 这里代码只是用于学习和交流场景上
//

// create atom write and read lock
// struct rwlock need zero is atom interface extend
// need var init struct rwlock rw = { 0,0 };
struct rwlock {
    atomic_bool wlock;
    atomic_long rlock;
};

// atomic_w_lock - add write lock
extern void atomic_w_lock(struct rwlock * rw) {
    // write lock
    while (!atomic_bool_compare_exchange_weak(&rw->wlock)) {}
    // 等待读锁释放
    while (atomic_load(&rw->rlock)) {}
}

// atomic_r_lock - add read lock
extern void atomic_r_lock(struct rwlock * rw) {
    for (;;) {
        // 等待读完毕, 并设置内存屏障
        while (atomic_load(&rw->wlock)) {}

        // 乐观的添加读计数
        atomic_fetch_add(&rw->rlock, 1);

        // 没有写占用, 就可以读了
        if (!atomic_load(&rw->wlock))
            break;

        // 还是有写, 收回刚添加的读计数
        atomic_fetch_sub(&rw->rlock, 1);
    }
}

// atomic_w_unlock - unlock write lock
extern void atomic_w_unlock(struct rwlock * rw) {
    assert(atomic_load(&rw->wlock));
    atomic_store(&rw->wlock, false);
}

// atomic_r_unlock - unlock read lock
extern void atomic_r_unlock(struct rwlock * rw) {
    assert(atomic_load(&rw->rlock));
    atomic_fetch_sub(&rw->rlock, 1);
}

// atomic_r_trylock - try add read lock
extern bool atomic_r_trylock(struct rwlock * rw) {
    if (atomic_load(&rw->wlock))
        return false;
    
    // 乐观的添加读计数
    atomic_fetch_add(&rw->rlock, 1);
    
    if (atomic_load(&rw->wlock)) {
        // 还是有写, 收回刚添加的读计数
        atomic_fetch_sub(&rw->rlock, 1);
        return false;
    }

    return true;
}

// atomic_w_trylock - try add write lock
extern bool atomic_w_trylock(struct rwlock * rw) {
    // 存在读锁, 直接返回
    if (atomic_load(&rw->rlock)) {
        return false;
    }

    // 尝试抢占写锁
    if (!atomic_bool_compare_exchange_strong(&rw->wlock)) {
        return false;
    }

    // 存在读锁, 释放申请到写锁, 直接返回
    if (atomic_load(&rw->rlock)) {
        // 存在读锁, 释放写锁
        atomic_store(&rw->wlock, false);
        return false;
    }

    return true;
}
