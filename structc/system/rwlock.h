#ifndef _H_RWLOCK
#define _H_RWLOCK

#include <atom.h>

//
// create atom write and read lock
// struct rwlock need zero
// is atom interface ext
//
struct rwlock {
    atom_t rlock;
    atom_t wlock;
};

// unlock write lock
inline void rwlock_unwlock(struct rwlock * lock) {
    atom_unlock(lock->wlock);
}

// unlock read lock
inline void rwlock_unrlock(struct rwlock * lock) {
    ATOM_DEC(lock->rlock);
}

// add read lock
inline void rwlock_rlock(struct rwlock * lock) {
    for (;;) {
        // 看是否有人在试图读, 得到并防止代码位置优化
        while (lock->wlock)
            atom_sync();

        ATOM_INC(lock->rlock);
        // 没有写占用, 开始读了
        if (!lock->wlock)
            break;

        // 还是有写, 删掉添加的读
        ATOM_DEC(lock->rlock);
    }
}

// add write lock
inline void rwlock_wlock(struct rwlock * lock) {
    atom_lock(lock->wlock);
    // 等待读占用锁
    while (lock->rlock)
        atom_sync();
}

#endif//_H_RWLOCK
