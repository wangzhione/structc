#include "rsq.h"

#define RSQ_INT         (256)

//
// tail == -1 (head == 0) => q empty
// push head = (tail + 1) % size => q full
//
struct rsq {
    atom_t lock;    // 原子锁
    uint32_t sz;    // q.sz is msg header body size length
    char * buff;    // 数据流
    int size;       // 队列大小
    int head;       // 头节点
    int tail;       // 尾节点
};

// rsq_len - q elemt len
inline int rsq_len(rsq_t q) {
    int tail = q->tail;
    if (tail < 0)
        return 0;
    
    tail = tail - q->head + 1;
    return tail < 0 ? tail + q->size : tail;
}

// rsq_expand 
static void rsq_expand(rsq_t q, int sz) {

}

//
// rsq_create - msg q 创建
// limit    : -1 不限制容量 0 默认容量 > 0 指定容量
// return   : msg q
//
inline rsq_t 
rsq_create(int limit) {
    struct rsq * q = malloc(sizeof(struct rsq));
    q->lock = 0;
    q->sz = 0;
    q->buff = malloc(q->size = RSQ_INT);
    q->head = 0;
    q->tail =-1;
    return q;
}

//
// rsq_delete - msg q 删除
// q        : msg q
// return   : void
//
inline void 
rsq_delete(rsq_t q) {
    if (q) {
        free(q->buff);
        free(q);
    }
}
