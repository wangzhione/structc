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
extern void rwlock_rlock(struct rwlock * lock);
// add write lock
extern void rwlock_wlock(struct rwlock * lock);

#endif//_H_RWLOCK
