#include "timer.h"

// timer_node 定时器节点
struct timer_node {
$LIST
    int id;            // 定时器 id
    void * arg;        // 执行函数参数
    node_f ftimer;     // 执行的函数事件
    struct timespec t; // 运行的具体时间
};

// timer_node id compare
inline static int timer_node_id_cmp(int id, 
                                    const struct timer_node * r) {
    return id - r->id;
}

// timer_node time compare 比较
inline static int timer_node_time_cmp(const struct timer_node * l, 
                                      const struct timer_node * r) {
    if (l->t.tv_sec != r->t.tv_sec)
        return (int)(l->t.tv_sec - r->t.tv_sec);
    return (int)(l->t.tv_nsec - r->t.tv_nsec);
}

// timer_list 链表对象管理器
struct timer_list {
    int id;                   // 当前 timer node id
    atom_t lock;              // 自旋锁
    volatile bool status;     // true is thread loop, false is stop
    struct timer_node * list; // timer list list
};

// timer_list_sus - 得到等待的微秒事件, <= 0 表示可以执行
inline int timer_list_sus(struct timer_list * list) {
    struct timespec * v = &list->list->t, t[1];
    timespec_get(t, TIME_UTC);
    return (int)((v->tv_sec - t->tv_sec) * 1000000 + 
        (v->tv_nsec - t->tv_nsec) / 1000);
}

// timer_list_run - 线程安全, 需要再 loop 之后调用
inline void timer_list_run(struct timer_list * list) {
    struct timer_node * node;
    atom_lock(list->lock);
    node = list->list;
    list->list = list_next(node);
    atom_unlock(list->lock);

    node->ftimer(node->arg);
    free(node);
}

// 定时器管理单例对象
static struct timer_list timer;

//
// timer_del - 删除定时器事件
// id       : 定时器 id
// return   : void
//
inline void 
timer_del(int id) {
    if (timer.list) {
        atom_lock(timer.lock);
        free(list_pop(&timer.list, timer_node_id_cmp, (void *)(intptr_t)id));
        atom_unlock(timer.lock);
    }
}

// timer_new - timer_node 定时器节点构建
static struct timer_node * timer_new(int s, node_f ftimer, void * arg) {
    struct timer_node * node = malloc(sizeof(struct timer_node));
    node->id = atom_inc(timer.id);
    if (node->id < 0)
        node->id = atom_and(timer.id, INT_MAX);
    node->arg = arg;
    node->ftimer = ftimer;
    timespec_get(&node->t, TIME_UTC);
    node->t.tv_sec += s / 1000;
    // nano second
    node->t.tv_nsec += (s % 1000) * 1000000;
    return node;
}

// 运行的主 loop, 基于 timer 管理器 
static void timer_run(struct timer_list * list) {
    // 正常轮循, 检查时间
    while (list->list) {
        int sus = timer_list_sus(list);
        if (sus > 0) {
            usleep(sus);
            continue;
        }

        timer_list_run(list);
    }

    // 已经运行结束
    list->status = false;
}

//
// timer_add - 添加定时器事件
// ms       : 执行间隔毫秒, <= 0 表示立即执行
// ftimer   : node_f 定时器行为
// arg      : 定时器参数
// return   : 定时器 id, < 0 标识 error
//
int 
timer_add(int ms, void * ftimer, void * arg) {
    if (ms <= 0) {
        ((node_f)ftimer)(arg);
        return 0;
    }

    struct timer_node * node = timer_new(ms, ftimer, arg);
    int id = node->id;
    atom_lock(timer.lock);

    list_add(&timer.list, timer_node_time_cmp, node);

    // 判断是否需要开启新的线程
    if (!timer.status) {
        if (!pthread_async(timer_run, &timer))
            timer.status = true;
        else {
            free(node);
            id = -1;
        }
    }

    atom_unlock(timer.lock);
    return id;
}
