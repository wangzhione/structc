#include "rwlock.h"

// add read lock
void 
rwlock_rlock(struct rwlock * lock) {
    for (;;) {
        // 等待读完毕, 并防止代码位置优化
        while (lock->wlock)
            atom_sync();

        atom_inc(lock->rlock);
        // 没有写占用, 开始读了
        if (!lock->wlock)
            break;

        // 还是有写, 删掉添加的读
        atom_dec(lock->rlock);
    }
}

// add write lock
void 
rwlock_wlock(struct rwlock * lock) {
    atom_lock(lock->wlock);
    // 等待读占用锁
    while (lock->rlock)
        atom_sync();
}
