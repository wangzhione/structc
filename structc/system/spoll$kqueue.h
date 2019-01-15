#if !defined(_SPOLL$KQUEUE_H) && !defined(_WIN32) && !defined(__linux__)
#define _SPOLL$KQUEUE_H

#include <sys/event.h>

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
    struct kevent t;
    EV_SET(&t, s, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(p, &t, 1, NULL, 0, NULL);
    EV_SET(&t, s, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    kevent(p, &t, 1, NULL, 0, NULL);
}

bool s_add(poll_t p, socket_t s, void * u) {
    struct kevent t;
    EV_SET(&t, s, EVFILT_READ, EV_ADD, 0, 0, u);
    if (kevent(p, &t, 1, NULL, 0, NULL) < 0 || t.flags & EV_ERROR)
        return true;

    EV_SET(&t, s, EVFILT_WRITE, EV_ADD, 0, 0, u);
    if (kevent(p, &t, 1, NULL, 0, NULL) < 0 || t.flags & EV_ERROR) {
        EV_SET(&t, s, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(p, &t, 1, NULL, 0, NULL);
        return true;
    }
    EV_SET(&t, s, EVFILT_WRITE, EV_DISABLE, 0, 0, u);
    if (kevent(p, &t, 1, NULL, 0, NULL) < 0 || t.flags & EV_ERROR) {
        s_del(p, sock);
        return 1;
    }
    return false;
}

inline void s_write(poll_t p, socket_t s, void * u, bool enable) {
    struct kevent t;
    EV_SET(&t, s, EVFILT_WRITE, enable ? EV_ENABLE : EV_DISABLE, 0, 0, u);
    if (kevent(p, &t, 1, NULL, 0, NULL) < 0 || t.flags & EV_ERROR)
        CERR("kevent error %p, %d, %d", u, enable, t.flags);
}

//
// s_wait   - wait 函数, 守株待兔
// p        : poll 对象
// e        : 返回操作事件集
// return   : 返回操作事件长度, < 0 表示失败
//
int s_wait(poll_t p, event_t e) {
    struct kevent v[MAX_EVENT];
    int n = kevent(p, NULL, 0, v, sizeof v / sizeof *v, NULL);

    for (int i = 0; i < n; ++i) {
        unsigned filter = v[i].filter;
        bool eof = v[i].flags & EV_EOF;
        e[i].u = v[i].udata;
        e[i].write = filter == EVFILT_WRITE && !eof;
        e[i].read = filter == EVFILT_READ && !eof;
        e[i].error = v[i].flags & EV_ERROR;
        e[i].eof = eof;
    }

    return n;
}

#endif//_SPOLL$KQUEUE_H
