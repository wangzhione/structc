#ifndef _MQ_H
#define _MQ_H

#include "q.h"
#include "atom.h"

struct mq {
    q_t q;             // 队列
    atom_t lock;       // 自旋锁
};

typedef struct mq * mq_t;

//
// mq_delete - 消息队列删除
// q        : 消息队列对象
// fdie     : node_f 行为, 删除 push 进来的节点
// return   : void
//
inline void mq_delete(mq_t q, node_f fdie) {
    // 销毁所有对象
    q_delete(q->q, fdie);
    free(q);
}

//
// mq_create - 消息队列创建
// return   : 消息队列对象
//
inline mq_t mq_create(void) {
    struct mq * q = malloc(sizeof(struct mq));
    q_init(q->q);
    q->lock = 0;
    return q;
}

//
// mq_pop - 消息队列中弹出消息, 并返回数据
// q        : 消息队列对象
// return   : 若 mq empty return NULL
//
inline void * mq_pop(mq_t q) {
    atom_lock(q->lock);
    void * m = q_pop(q->q);
    atom_unlock(q->lock);
    return m;
}

//
// mq_push - 消息队列中压入数据
// q        : 消息队列对象
// m        : 压入的消息
// return   : void
//
inline void mq_push(mq_t q, void * m) {
    atom_lock(q->lock);
    q_push(q->q, m);
    atom_unlock(q->lock);
}

//
// mq_len - 消息队列的长度
// q        : 消息队列对象
// return   : 返回消息队列长度
//
inline static int mq_len(mq_t q) {
    int head, tail, size;
    atom_lock(q->lock);
    if ((tail = q->q->tail) == -1) {
        atom_unlock(q->lock);
        return 0;
    }

    head = q->q->head;
    size = q->q->size;
    atom_unlock(q->lock);

    // 计算当前时间中内存队列的大小
    tail -= head - 1;
    return tail>0 ? tail : tail+size;
}

#endif//_MQ_H
