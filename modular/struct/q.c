#include "q.h"

// Q_INT  - 队列初始大小, 必须是 2 的幂
#define Q_INT     (1<< 6)

bool q_init(struct q * q) {
    void * data = malloc(sizeof(void *) * Q_INT);
    if (data == nullptr) {
        RETURN(false, "malloc panic Q_INT = %d", Q_INT);
    }

    q->head = 0;
    q->tail = -1;
    q->cap = Q_INT;
    q->data = data;
    return true;
}

//
// q_pop - 队列中弹出消息数据
// q      : 队列对象
// return : 若队列 empty, 返回 nullptr
//
void * 
q_pop(struct q * q) {
    if (q_exist(q)) {
        void * m = q->data[q->head];
        if (q->tail != q->head)
            q->head = (q->head+1) & (q->cap-1);
        else {
            // empty 情况, 重置 tail 和 head
            q->tail = -1; q->head =  0;
        }
        return m;
    }
    return nullptr;
}

// q_expand - expand memory by twice
static bool q_expand(struct q * q) {
    assert(q != nullptr && q->cap > 0);

    int cap = q->cap << 1;
    void ** p = malloc(sizeof(void *) * cap);
    if (p == nullptr) {
        RETURN(false, "malloc panic cap = %d", cap); 
    }

    for (int i = 0; i < q->cap; ++i)
        p[i] = q->data[(q->head+i) & (q->cap-1)];
    free(q->data);

    // 重新构造内存关系
    q->head = 0;
    q->tail = q->cap;
    q->cap = cap;
    q->data = p;

    return true;
}

//
// q_push - 队列中压入数据
// q      : 队列对象
// m      : 压入消息
// return : bool true push success
// 
bool 
q_push(struct q * q, void * m) {
    int tail = (q->tail+1) & (q->cap-1);
    // 队列 full 直接扩容
    if (tail == q->head && q->tail >= 0) {
        if (q_expand(q) == false) {
            return false;
        }
    } else {
        q->tail = tail;   
    }
    q->data[q->tail] = m;
    return true;
}

//
// q_delete - 队列删除
// q        : 队列对象
// fdie     : node_f push 结点删除行为
// return   : void
//
void 
q_delete(struct q * q, node_f fdie) {
    // 销毁所有对象
    if (fdie != nullptr && q_exist(q)) {
        for (;;) {
            fdie(q->data[q->head]);
            if (q->head == q->tail)
                break;
            q->head = (q->head+1) & (q->cap-1);
        }
    }

    free(q->data);
}
