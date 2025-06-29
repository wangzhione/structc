#pragma once

#include "struct.h"

 // q simple ringlike queue
// pop empty <=> tail == -1 ( head == 0 )
// push full <=> head == (tail + 1) % cap && tail >= 0
//
struct q {
    int     head;       // 头结点索引
    int     tail;       // 尾结点索引
    int      cap;       // 队列容量
    void ** data;       // 队列实体
};

extern bool q_init(struct q * q);

inline void q_release(struct q * q) {
    free(q->data);
}

inline bool q_empty(struct q * q) {
    return q->tail < 0;
}

inline bool q_exist(struct q * q) {
    return q->tail >=0;
}

inline int q_len(struct q * q) {
    // return q->tail < 0 ? 0 :( 
    //  q->tail < q->head ? q->cap+q->tail-q->head+1 : q->tail-q->head+1
    // );
    // q->tail >= 0       ? 1 : 0
    // q->tail < q-> head ? 1 : 0
    return (q->tail >= 0)*((q->tail < q->head)*q->cap + q->tail-q->head+1);
}

//
// q_swap - q swap
// r      : q one
// w      : q two
// return : void
//
#define q_swap(r, w) \
do { struct q $p = r; r = w; w = $p; } while(0)

//
// q_pop - 队列中弹出消息数据
// q      : 队列对象
// return : 若队列 empty, 返回 nullptr
//
extern void * q_pop(struct q * q);

//
// q_push - 队列中压入数据
// q      : 队列对象
// m      : 压入消息
// return : bool true push success
// 
extern bool q_push(struct q * q, void * m);

//
// q_delete - 队列删除
// q        : 队列对象
// fdie     : node_f push 结点删除行为
// return   : void
//
extern void q_delete(struct q * q, node_f fdie);
