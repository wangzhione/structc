#include <mq.h>
#include <atom.h>

// 初始化队列的大小, 必须是 2 的幂
#define _INT_MQ     (1<<6)

//
// pop empty  <=> tail == -1 ( head == 0 )
// push full  <=> head == tail + 1
//
struct mq {
    int head;           // 头结点
    int tail;           // 尾结点
    int size;           // 队列大小
    void ** queue;      // 队列实体

    atom_t lock;        // 队列原子锁
    volatile bool fee;  // true表示销毁状态
};

//
// mq_create - 消息队列创建
// return	: 创建好的消息队列
//
inline mq_t 
mq_create(void) {
    struct mq * q = malloc(sizeof(struct mq));
    q->head = 0;
    q->tail = -1;
    q->size = _INT_MQ;
    q->queue = malloc(sizeof(void *) * q->size);

    q->lock = 0;
    q->fee = false;
    return q;
}

//
// mq_delete - 消息队列销毁
// q		: 消息队列对象
// fdie		: 删除 push 进来的结点
// return	: void
//
void 
mq_delete(mq_t q, node_f fdie) {
    if (!q || q->fee) 
        return;
    
    atom_lock(q->lock);
    q->fee = true;

    // 销毁所有对象
    if (q->tail >= 0 && fdie) {
        for (;;) {
            fdie(q->queue[q->head]);
            if (q->head == q->tail)
                break;
            q->head = (q->head + 1) & (q->size - 1);
        }
    }

    free(q->queue);
    atom_unlock(q->lock);

    free(q);
}

// add two cap memory, memory is do not have assert
static void _mq_expand(struct mq * q) {
    int i, size = q->size << 1;
    void ** nq = malloc(sizeof(void *) * size);
    for (i = 0; i < q->size; ++i)
        nq[i] = q->queue[(q->head + i) & (q->size - 1)];
    free(q->queue);

    // 重新构造内存关系
    q->head = 0;
    q->tail = q->size;
    q->size = size;
    q->queue = nq;
}

//
// mq_push - 消息队列中压入数据
// q		: 消息队列对象
// m		: 压入的消息
// return	: void
// 
void 
mq_push(mq_t q, void * m) {
    int tail;
    if (!q || q->fee || !m)
        return;

    atom_lock(q->lock);

    tail = (q->tail + 1) & (q->size - 1);
    // 队列 full 直接扩容
    if (tail == q->head && q->tail >= 0)
        _mq_expand(q);
    else
        q->tail = tail;
    q->queue[q->tail] = m;

    atom_unlock(q->lock);
}

//
// mq_pop - 消息队列中弹出消息,并返回数据
// q		: 消息队列对象
// return	: 返回队列尾巴, 队列 empty时候, 返回NULL
//
void * 
mq_pop(mq_t q) {
    void * m = NULL;
    if (!q || q->fee)
        return m;

    atom_lock(q->lock);
    if (q->tail >= 0) {
        m = q->queue[q->head];
        if (q->tail != q->head)
            q->head = (q->head + 1) & (q->size - 1);
        else {
            q->tail = -1;
            q->head = 0; // empty 情况, 重置 tail and head
        }
    }
    atom_unlock(q->lock);

    return m;
}

//
// mq_len - 消息队列的长度
// q		: 消息队列对象
// return	: 返回消息队列长度
// 
int 
mq_len(mq_t q) {
    int head, tail, size;
    if (!q || q->fee)
        return 0;

    atom_lock(q->lock);
    tail = q->tail;
    if (tail == -1) {
        atom_unlock(q->lock);
        return 0;
    }

    head = q->head;
    size = q->size;
    atom_unlock(q->lock);

    // 计算当前时间中内存队列的大小
    tail -= head - 1;
    return tail > 0 ? tail : tail + size;
}
