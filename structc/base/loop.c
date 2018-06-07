#include <loop.h>

// loop 轮询器结构
struct loop {
    q_t rq;             // 读消息
    q_t wq;             // 写消息
    atom_t lock;        // 消息切换锁
    node_f frun;        // 消息处理行为
    node_f fdie;        // 消息销毁行为
    sem_t block;        // 线程信号量
    pthread_t id;       // 运行的线程id
    volatile bool loop; // true 线程正在运行
    volatile bool wait; // true 线程空闲等待
};

// 轮询器执行的循环体
static void _loop(struct loop * p) {
    while (p->loop) {
        void * m = q_pop(p->rq);
        if (NULL == m) {
            atom_lock(p->lock);
            q_swap(p->rq, p->wq);
            atom_unlock(p->lock);
            m = q_pop(p->rq);
        }

        // 仍然没有数据, 开始睡眠
        if (NULL == m) {
            p->wait = true;
            sem_wait(&p->block);
        } else {
            // 开始处理消息
            p->frun(m);
            p->fdie(m);
        }
    }
}

//
// loop_create - 创建轮询对象
// frun     : 消息处理行为
// fdie     : 消息销毁行为
// return   : 轮询对象
//
inline loop_t 
loop_create_(node_f frun, node_f fdie) {
    struct loop * p = malloc(sizeof(struct loop));
    q_init(p->rq);
    q_init(p->wq);
    p->lock = 0;
    p->frun = frun;
    p->fdie = fdie;
    p->loop = true;
    p->wait = true;
    // 初始化 POSIX 信号量, 进程内线程共享, 初始值 0
    sem_init(&p->block, 0, 0);
    if (pthread_run(&p->id, _loop, p)) {
        sem_destroy(&p->block);
        free(p->rq->queue);
        free(p->wq->queue);
        free(p); p = NULL;
    }
    return p;
}

