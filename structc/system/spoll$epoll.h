#if !defined(_SPOLL$EPOLL_H) && defined(__linux__)
#define _SPOLL$EPOLL_H

#include "spoll.h"
#include <sys/epoll.h>

//
// s_create   - 创建 poll 对象
// s_invalid  - true 表示创建 poll 对象异常
// s_delete   - 销毁创建的 poll 对象
//
inline poll_t s_create(void) {
    return epoll_create1(EPOLL_CLOEXEC);
}

inline bool s_invalid(poll_t p) {
    return p < 0;
}

inline void s_delete(poll_t p) {
    close(p);
}

//
// s_del     - 删除监测的 socket
// s_add     - 添加监测的 socket, 并设置读模式, 失败返回 true
// s_write   - 修改监测的 socket, 通过 enable = true 设置写模式
//
inline void s_del(poll_t p, socket_t s) {
    epoll_ctl(p, EPOLL_CTL_DEL, s, NULL);
}

inline bool s_add(poll_t p, socket_t s, void * u) {
    struct epoll_event t;
    t.events = EPOLLIN;
    t.data.ptr = u;
    return epoll_ctl(p, EPOLL_CTL_ADD, s, &t);
}

inline void s_write(poll_t p, socket_t s, void * u, bool enable) {
    struct epoll_event t;
    t.events = enable ? EPOLLIN | EPOLLOUT : EPOLLIN;
    t.data.ptr = u;
    epoll_ctl(p, EPOLL_CTL_MOD, s, &t);
}

//
// s_wait   - wait 函数, 守株待兔
// p        : poll 对象
// e        : 返回操作事件集
// return   : 返回操作事件长度, < 0 表示失败
//
int s_wait(poll_t p, event_t e) {
    struct epoll_event v[MAX_EVENT];
    int n = epoll_wait(p, v, sizeof v / sizeof *v, -1);

    for (int i = 0; i < n; ++i) {
        uint32_t flag = v[i].events;
        e[i].u = v[i].data.ptr;
        e[i].read = flag & (EPOLLIN | EPOLLHUP);
        e[i].write = flag & EPOLLOUT;
        e[i].error = flag & EPOLLERR;
        e[i].eof = false;
    }

    return n;
}

#endif//_SPOLL$EPOLL_H
