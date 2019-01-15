#ifndef _SPOLL_H
#define _SPOLL_H

#include "socket.h"

// struct event[MAX_EVENT] -> event_t 事件集
//
struct event {
    void * u;
    bool read;
    bool write;
    bool error;
    bool eof;
};

#define MAX_EVENT (64)

typedef struct event event_t[MAX_EVENT];

#ifdef _WIN32
typedef struct poll * poll_t;
#else
typedef int poll_t;
#endif

//
// s_create   - 创建 poll 对象
// s_invalid  - true 表示创建 poll 对象异常
// s_delete   - 销毁创建的 poll 对象
//
extern poll_t s_create(void);
extern bool s_invalid(poll_t p);
extern void s_delete(poll_t p);

//
// s_del     - 删除监测的 socket
// s_add     - 添加监测的 socket, 并设置读模式, 失败返回 true
// s_write   - 修改监测的 socket, 通过 enable = true 设置写模式
//
extern void s_del(poll_t p, socket_t s);
extern bool s_add(poll_t p, socket_t s, void * u);
extern void s_write(poll_t p, socket_t s, void * u, bool enable);

//
// s_wait   - wait 函数, 守株待兔
// p        : poll 对象
// e        : 返回操作事件集
// return   : 返回操作事件长度, < 0 表示失败
//
extern int s_wait(poll_t p, event_t e);

#endif//_SPOLL_H
