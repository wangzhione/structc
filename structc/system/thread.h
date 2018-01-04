#ifndef _H_THREAD
#define _H_THREAD

#include <assext.h>
#include <struct.h>
#include <pthread.h>
#include <semaphore.h>

#ifdef __GNUC__
#   define PTHREAD_ERROR    (-1)
#endif

#ifdef _MSC_VER
#   define PTHREAD_ERROR    ((ptw32_handle_t) { NULL, -1 })
#endif

//
// pthread_error - 判断线程 id 是否异常
// tid      : 线程 id
// return   : true 表示异常
//
inline bool pthread_error(pthread_t tid) {
    return pthread_equal(tid, PTHREAD_ERROR);
}

//
// pthread_run - 异步启动线程
// frun     : 运行的主体
// arg      : 运行参数
// return   : 返回线程运行 id 
//
#define pthread_run(frun, arg) \
pthread_run_((node_f)(frun), (void *)(intptr_t)(arg))

inline pthread_t pthread_run_(node_f frun, void * arg) {
    pthread_t tid;
    if (pthread_create(&tid, NULL, (start_f)frun, arg) < 0) {
        CERR("pthread_create err %p, %p.", frun, arg);
        tid = PTHREAD_ERROR;
    }
    return tid;
}

//
// pthread_end - 等待启动线程结束
// tid      : 线程id
// return   : void
//
inline void pthread_end(pthread_t tid) {
    pthread_join(tid, NULL);
}

//
// pthread_async - 异步启动分离线程
// frun     : 运行的主体
// arg      : 运行参数
// return   : 返回线程运行 id 
// 
#define pthread_async(frun, arg) \
pthread_async_((node_f)(frun), (void *)(intptr_t)(arg))

inline pthread_t pthread_async_(node_f frun, void * arg) {
    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid, &attr, (start_f)frun, arg) < 0) {
        CERR("pthread_create err %p, %p.", frun, arg);
        tid = PTHREAD_ERROR;
    }
    pthread_attr_destroy(&attr);

    return tid;
}

#endif//_H_THREAD
