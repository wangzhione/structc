#pragma once

#include "atom.h"

// create atom write and read lock
// struct rwlock need zero is atom interface extend
// need var init struct rwlock rw = { 0,0 };
struct rwlock {
    atom_t rlock;
    atom_t wlock;
};

// rwlock_r_lock - add read lock
extern void rwlock_r_lock(struct rwlock * rw);

// rwlock_w_lock - add write lock
extern void rwlock_w_lock(struct rwlock * rw);

// rwlock_w_unlock - unlock write lock
extern void rwlock_w_unlock(struct rwlock * rw);

// rwlock_r_unlock - unlock read lock
extern void rwlock_r_unlock(struct rwlock * rw);

// rwlock_r_trylock - try add read lock
extern bool rwlock_r_trylock(struct rwlock * rw);

// rwlock_w_trylock - try add write lock
extern bool rwlock_w_trylock(struct rwlock * rw);
