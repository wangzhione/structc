#ifndef _THREAD_H
#define _THREAD_H

#include "struct.h"
#include <pthread.h>
#include <semaphore.h>

//
// pthread_end - 等待线程运行结束
// tid      : 线程 id
// return   : void
//
inline void pthread_end(pthread_t id) {
    pthread_join(id, NULL);
}

//
// pthread_run - 启动线程
// id       : 线程 id
// frun     : 运行主体
// arg      : 运行参数
// return   : return 0 is success
//
#define pthread_run(id, frun, arg)                                  \
pthread_run_(&(id), (node_f)(frun), (void *)(intptr_t)(arg))
inline int pthread_run_(pthread_t * pi, node_f frun, void * arg) {
    return pthread_create(pi, NULL, (start_f)frun, arg);
}

//
// pthread_async - 启动无需等待的线程
// frun     : 运行的主体
// arg      : 运行参数
// return   : return 0 is success
// 
#define pthread_async(frun, arg)                                    \
pthread_async_((node_f)(frun), (void *)(intptr_t)(arg))
inline int pthread_async_(node_f frun, void * arg) {
    pthread_t id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&id, &attr, (start_f)frun, arg);
    pthread_attr_destroy(&attr);
    return ret;
}

#endif//_THREAD_H
