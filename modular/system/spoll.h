#pragma once

#include "socket.h"

#define MAX_EVENT (64)

struct spoll_event {
    void * u;
    bool read;
    bool write;
    bool error;
    bool eof;
};

// poll 模型对象
#if defined(__linux__) && defined(__GNUC__)
typedef int            spoll_t;
#elif defined(_WIN32) && defined(_MSC_VER)
typedef struct spoll * spoll_t;
#endif

// poll 模型 event 事件集
typedef struct spoll_event spoll_event_t[MAX_EVENT];

//
// spoll_create   - 创建 poll 对象
// spoll_invalid  - true 表示创建 poll 对象异常
// spoll_delete   - 销毁创建的 poll 对象
//
extern spoll_t spoll_create(void);
extern bool spoll_invalid(spoll_t p);
extern void spoll_delete(spoll_t p);

//
// spoll_del     - 删除监测的 socket fd
// spoll_add     - 添加监测的 socket fd, 并设置读模式, 失败返回 true
// spoll_mod     - 修改监测的 socket fd, 通过 true 和 false 设置读写
//
extern void spoll_del(spoll_t p, socket_t s);
extern bool spoll_add(spoll_t p, socket_t s, void * u);
extern int spoll_mod(spoll_t p, socket_t s, void * u, bool read, bool write);

//
// spoll_wait    - wait 函数, 守株待兔
// p        : poll 对象
// e        : 返回操作事件集
// return   : 返回操作事件长度, < 0 表示失败
//
extern int spoll_wait(spoll_t p, spoll_event_t e);
