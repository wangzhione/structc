#include "rwlock.h"

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
// atomic 封印在学习和特殊业务场景上
//

// rwlock_rlock - add read lock
void
rwlock_r_lock  (struct rwlock * rw) {
    for (;;) {
        // 等待读完毕, 并设置内存屏障
        while (rw->wlock)
            atom_sync();

        // 乐观的添加读计数
        atom_inc(rw->rlock);

        // 没有写占用, 就可以读了
        if (!rw->wlock)
            break;

        // 还是有写, 收回刚添加的读计数
        atom_dec(rw->rlock);
    }
}

// rwlock_wlock - add write lock
void
rwlock_w_lock  (struct rwlock * rw) {
    atom_lock(rw->wlock);
    // 等待读锁释放
    while (rw->rlock)
        atom_sync();
}

// rwlock_unwlock - unlock write lock
inline void
rwlock_w_unlock(struct rwlock * rw) {
    assert(rw->wlock > 0);
    atom_unlock(rw->wlock);
}

// rwlock_unrlock - unlock read lock
inline void
rwlock_r_unlock(struct rwlock * rw) {
    assert(rw->rlock > 0);
    atom_dec(rw->rlock);
}

// rwlock_r_trylock - try add read lock
inline bool 
rwlock_r_trylock(struct rwlock * rw) {
    if (rw->wlock)
        return false;

    atom_inc(rw->rlock);

    if (rw->wlock) {
        atom_dec(rw->rlock);
        return false;
    }

    return true;
}

// rwlock_w_trylock - try add write lock
inline bool 
rwlock_w_trylock(struct rwlock * rw) {
    if (atom_trylock(rw->wlock) && rw->rlock) {
        atom_unlock(rw->wlock);
        return false;
    }
    return true;
}
