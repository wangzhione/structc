#include "loop.h"

// loop 轮询器结构
struct loop {
    pthread_t id;           // 运行的线程id
    pthread_mutex_t lock;   // 消息切换锁
    pthread_cond_t cond;    // 等待条件变量

    volatile bool wait;     // true 标识正在等待
    volatile bool loop;     // true 线程正在运行

    struct q wq;            // 写消息

    struct q rq;            // 读消息
    node_f frun;            // 消息处理行为
    node_f fdie;            // 消息销毁行为
};

// loop_run 消息处理行为
inline void loop_run(loop_t p, void * m) {
    // 开始处理消息
    p->frun(m);
    p->fdie(m);
}

//
// loop_delete - 轮询对象销毁
// p        : 轮询对象
// return   : void
//
void 
loop_delete(loop_t p) {
    if (p == NULL) return;

    //
    // delete 执行必须在 push 之后, C 代码是在刀剑上跳舞 ~ 
    //
    p->loop = false;
    // 尝试激活信号量
    pthread_cond_broadcast(&p->cond);
    // 等待线程结束, 然后退出
    pthread_end(p->id);

    // 队列内存清理
    q_delete(&p->wq, p->fdie);
    q_delete(&p->rq, p->fdie);
    free(p);
}

//
// loop_push - 消息压入轮询器
// p        : 轮询对象
// m        : 消息
// return   : void
//
void 
loop_push(loop_t p, void * m) {
    assert(p != NULL && m != NULL);

    pthread_mutex_lock(&p->lock);

    q_push(&p->wq, m);

    if (p->wait) {
        p->wait = false;
        pthread_cond_signal(&p->cond);
    }

    pthread_mutex_unlock(&p->lock);
}

// loop_loop 轮询器执行的循环体
static void loop_loop(loop_t p) {
    while (p->loop) {
        void * m = q_pop(&p->rq);
        if (m != NULL) {
            loop_run(p, m);
            continue;
        }

        pthread_mutex_lock(&p->lock);
        // 没有数据开始阻塞
        if (q_empty(&p->wq)) {
            p->wait = true;
            pthread_cond_wait(&p->cond, &p->lock);
        }

        // read q <- write q
        q_swap(p->rq, p->wq);

        pthread_mutex_unlock(&p->lock);
    }
}

//
// loop_create - 创建轮询对象
// frun     : node_f 消息处理行为
// fdie     : node_f 消息销毁行为
// return   : 轮询对象
//
loop_t 
loop_create(void * frun, void * fdie) {
    assert(frun != NULL);

    loop_t p = malloc(sizeof(struct loop));
    if (p != NULL) {
        RETNUL("malloc panic %zu", sizeof(struct loop));
    }

    // 初始化 POSIX 互斥锁和条件变量
    if (pthread_mutex_init(&p->lock, NULL)) {
        free(p);
        RETNUL("pthread_mutex_init panic error");
    }
    if (pthread_cond_init(&p->cond, NULL)) {
        pthread_cond_destroy(&p->cond);
        free(p);
        RETNUL("pthread_cond_init panic error");
    }

    if (q_init(&p->rq) == false) {
        pthread_cond_destroy(&p->cond);
        pthread_mutex_destroy(&p->lock);
        free(p);
        return NULL;
    }
    if (q_init(&p->wq) == false) {
        free(p->rq.data);
        pthread_cond_destroy(&p->cond);
        pthread_mutex_destroy(&p->lock);
        free(p);
        return NULL;
    }
    p->frun = frun;
    p->fdie = fdie;
    p->wait = p->loop = true;

    if (pthread_run(&p->id, loop_loop, p)) {
        free(p->wq.data);
        free(p->rq.data);
        pthread_cond_destroy(&p->cond);
        pthread_mutex_destroy(&p->lock);
        free(p);
        RETNUL("pthread_run panic error");
    }
    
    return p;
}
