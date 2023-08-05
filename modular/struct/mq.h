#pragma once

#include "q.h"
#include "spinlock.h"

//
// init : 
//    struct mq mq;
//    if (q_init(&mq.q)) {
//        mq.lock = (atomic_flag) ATOMIC_FLAG_INIT;
//    }
// free :
//    q_delete(&q->q, fdie);    
// 
struct mq {
    struct q       q;   // 队列
    atomic_flag lock;   // 自旋锁
};

//
// mq_pop - 消息队列中弹出消息, 并返回数据
// q        : 消息队列对象
// return   : 若 mq empty return NULL
//
inline void * mq_pop(struct mq * q) {
    atomic_flag_lock(&q->lock);
    void * m = q_pop(&q->q);
    atomic_flag_unlock(&q->lock);
    return m;
}

//
// mq_push - 消息队列中压入数据
// q        : 消息队列对象
// m        : 压入的消息
// return   : void
//
inline void mq_push(struct mq * q, void * m) {
    atomic_flag_lock(&q->lock);
    q_push(&q->q, m);
    atomic_flag_unlock(&q->lock);
}

//
// mq_len - 消息队列的长度
// q        : 消息队列对象
// return   : 返回消息队列长度
//
extern inline int mq_len(struct mq * q) {
    atomic_flag_lock(&q->lock);
    int len = q_len(&q->q);
    atomic_flag_unlock(&q->lock);
    return len;
}
