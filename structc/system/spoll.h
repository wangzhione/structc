#ifndef _H_SPOLL
#define _H_SPOLL

#include <socket.h>

//
// 定义操作的事件集结构
// event[INT_EVENT] -> event_t
//
struct event {
    void * ud;
    bool read;
    bool write;
    bool error;
};

#define MAX_EVENT (64)

typedef struct event event_t[MAX_EVENT];

#if defined(__linux__)
    typedef int poll_t;
#else
    typedef struct poll * poll_t;
#endif

//
// s_create     - 创建一个 poll 模型
// s_error      - true 表示当前创建的 poll 模型有问题
// s_delete     - 销毁一个创建的 poll 模型
//
extern poll_t s_create(void);
extern bool s_error(poll_t p);
extern void s_delete(poll_t p);

//
// s_del        - 删除1检测的 socket
// s_add        - 添加监测的 socket, 并设置读模式, 失败返回 true
// s_write      - 修改当前 socket, 通过 enable = true 设置写模式
//
extern void s_del(poll_t p, socket_t s);
extern bool s_add(poll_t p, socket_t s, void * ud);
extern void s_write(poll_t p, socket_t s, void * ud, bool enable);

//
// s_wait       - wait 函数, 等待别人自投罗网
// p        : poll 模型
// e        : 返回的操作事件集
// return   : 返回待操作事件长度, < 0 表示失败
//
extern int s_wait(poll_t p, event_t e);

#endif//_H_SPOLL
