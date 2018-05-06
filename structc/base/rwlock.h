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

// add read lock
extern void rwlock_rlock(struct rwlock * lock);
// add write lock
extern void rwlock_wlock(struct rwlock * lock);

// unlock write
extern void rwlock_unwlock(struct rwlock * lock);
// add write lock
extern void rwlock_unrlock(struct rwlock * lock);

#endif//_H_RWLOCK
