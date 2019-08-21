#include "rwlock.h"

// rwlock_rlock - add read lock
void
rwlock_rlock(struct rwlock * rw) {
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
rwlock_wlock(struct rwlock * rw) {
    atom_lock(rw->wlock);
    // 等待读锁释放
    while (rw->rlock)
        atom_sync();
}

// rwlock_unwlock - unlock write lock
inline void
rwlock_unwlock(struct rwlock * rw) {
    assert(rw->wlock > 0);
    atom_unlock(rw->wlock);
}

// rwlock_unrlock - unlock read lock
inline void
rwlock_unrlock(struct rwlock * rw) {
    assert(rw->rlock > 0);
    atom_dec(rw->rlock);
}
