﻿#include "list.h"
#include "times.h"
#include "thread.h"

// timer_node 定时器结点
struct timer_node {
$LIST
    int id;            // 定时器 id
    void * arg;        // 执行函数参数
    node_f ftimer;     // 执行的函数事件
    struct timespec t; // 运行的具体时间
};

// timer_node id compare
static inline int timer_node_id_cmp(int id, 
                                    const struct timer_node * r) {
    return id - r->id;
}

// timer_node time compare 比较
static inline int timer_node_time_cmp(const struct timer_node * l, 
                                      const struct timer_node * r) {
    if (l->t.tv_sec != r->t.tv_sec)
        return (int)(l->t.tv_sec - r->t.tv_sec);
    return (int)(l->t.tv_nsec - r->t.tv_nsec);
}

// timer_list 链表对象管理器
struct timer_list {
    atomic_int id;            // 当前 timer node id
    atomic_flag lock;         // 自旋锁
    volatile bool status;     // true is thread loop, false is stop
    struct timer_node * list; // timer list list
};

// 定时器管理单例对象
static struct timer_list timer = { .id = 1, .lock = ATOMIC_FLAG_INIT };

// get atomic int 1 -> INT_MAX -> 1
static inline int timer_list_id() {
    // 0 -> INT_MAX -> INT_MIN -> 0
    int id = atomic_fetch_add(&timer.id, 1) + 1;
    if (id < 0) {
        // INT_MAX + 1 -> INT_MIN
        // 0x7F FF FF FF + 1 -> 0x80 00 00 00

        // INT_MIN & INT_MAX => 0x80 00 00 00 & 0x7F FF FF FF => 0x00 00 00 00
        // id = atomic_fetch_and(&timer.id, INT_MAX) & INT_MAX;
        // Multiple operations atomic_fetch_and can ensure timer.id >= 0
        atomic_fetch_and(&timer.id, INT_MAX);

        // again can ensure id >= 1
        id = atomic_fetch_add(&timer.id, 1) + 1;
    }
    return id;
}

// timer_list_sus - 得到等待的微秒事件, <= 0 表示可以执行
inline int timer_list_sus(struct timer_list * tl) {
    struct timespec * v = &tl->list->t, t[1];
    timespec_get(t, TIME_UTC);
    return (int)((v->tv_sec  - t->tv_sec ) * 1000000 + 
                 (v->tv_nsec - t->tv_nsec) / 1000);
}

// timer_list_run_node - 线程安全, 需要再 loop 之后调用
inline void timer_list_run_node(struct timer_list * tl) {
    atomic_flag_lock(&tl->lock);
    struct timer_node * node = tl->list;
    tl->list = list_next(node);
    atomic_flag_unlock(&tl->lock);

    node->ftimer(node->arg);
    free(node);
}

// 运行的主 loop, 基于 timer 管理器 
static void timer_list_run(struct timer_list * tl) {
    // 正常轮循, 检查时间
    while (tl->list) {
        int sus = timer_list_sus(tl);
        if (sus > 0) {
            usleep(sus);
            continue;
        }

        timer_list_run_node(tl);
    }

    // 已经运行结束
    tl->status = false;
}

// timer_new - timer_node 定时器结点构建
static struct timer_node * timer_new(int s, node_f ftimer, void * arg) {
    struct timer_node * node = malloc(sizeof(struct timer_node));
    node->id = timer_list_id();
    node->arg = arg;
    node->ftimer = ftimer;
    timespec_get(&node->t, TIME_UTC);
    node->t.tv_sec += s / 1000;
    // nano second
    node->t.tv_nsec += (s % 1000) * 1000000;
    return node;
}

//
// timer_add - 添加定时器事件
// ms        : 执行间隔毫秒, <= 0 表示立即执行
// ftimer    : node_f 定时器行为
// arg       : 定时器参数
// return    : 定时器 id, < 0 标识 error
//
int timer_add(int ms, void * ftimer, void * arg) {
    int id;
    struct timer_node * node;

    if (ms <= 0) {
        ((node_f)ftimer)(arg);
        return 0;
    }

    node = timer_new(ms, ftimer, arg);
    id = node->id;

    atomic_flag_lock(&timer.lock);

    list_add(&timer.list, timer_node_time_cmp, node);

    // 判断是否需要开启新的线程
    if (!timer.status) {
        if (pthread_async(timer_list_run, &timer)) {
            PERR("pthread_async id = %d", id);
            id = -1;
        } else {
            timer.status = true;
        }
    }

    atomic_flag_unlock(&timer.lock);

    // 尝试释放资源
    if (id == -1)
        free(node);
    return id;
}

//
// timer_del - 删除定时器事件
// id        : 定时器 id
// return    : void
//
void timer_del(int id) {
    struct timer_node * node;

    if (id <= 0 || timer.list == NULL) {
        POUT("id = %d, list = %p", id, timer.list);
        return;
    }

    atomic_flag_lock(&timer.lock);
    node = list_pop(&timer.list, timer_node_id_cmp, (void *)(intptr_t)id);
    atomic_flag_unlock(&timer.lock);

    free(node);
}
