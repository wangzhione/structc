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


