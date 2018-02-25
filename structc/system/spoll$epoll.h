#if defined(__linux__)

#include <spoll.h>
#include <sys/epoll.h>

//
// s_create     - 创建一个 poll 模型
// s_error      - true 表示当前创建的 poll 模型有问题
// s_delete     - 销毁一个创建的 poll 模型
//
inline poll_t 
s_create(void) {
    return epoll_create1(EPOLL_CLOEXEC);
}

inline bool 
s_error(poll_t p) {
    return p < SBase;
}

inline void 
s_delete(poll_t p) {
    close(p);
}

//
// s_del        - 删除1检测的 socket
// s_add        - 添加监测的 socket, 并设置读模式, 失败返回 true
// s_write      - 修改当前 socket, 通过 enable = true 设置写模式
//
inline void 
s_del(poll_t p, socket_t s) {
    epoll_ctl(p, EPOLL_CTL_DEL, s, NULL);
}

inline bool 
s_add(poll_t p, socket_t s, void * ud) {
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = ud;
    return !!epoll_ctl(p, EPOLL_CTL_ADD, s, &ev);
}

inline void 
s_write(poll_t p, socket_t s, void * ud, bool enable) {
    struct epoll_event ev;
    ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
    ev.data.ptr = ud;
    epoll_ctl(p, EPOLL_CTL_MOD, s, &ev);
}

//
// s_wait       - wait 函数, 等待别人自投罗网
// p        : poll 模型
// e        : 返回的操作事件集
// return   : 返回待操作事件长度, < 0 表示失败
//
int 
s_wait(poll_t p, event_t e) {
    struct epoll_event v[MAX_EVENT];
    int n = epoll_wait(p, v, sizeof(v) / sizeof(*v), -1);
    if (n <= 0) {
        RETURN(EBase, "epoll_wait n = %d error", n);
    }

    for (int i = 0; i < n; ++i) {
        uint32_t flag = v[i].events;
        e[i].ud = v[i].data.ptr;
        e[i].read = flag & (EPOLLIN | EPOLLHUP);
        e[i].write = flag & EPOLLOUT;
        e[i].error = flag & EPOLLERR;
    }

    return n;
}

#endif
