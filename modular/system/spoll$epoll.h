#pragma once

#if defined(__linux__) && defined(__GNUC__)

#include <sys/epoll.h>

#include "spoll.h"

inline spoll_t spoll_create(void) {
    return epoll_create1(EPOLL_CLOEXEC);
}

inline bool spoll_invalid(spoll_t p) {
    return p < 0;
}

inline void spoll_delete(spoll_t p) {
    close(p);
}

inline void spoll_del(spoll_t p, socket_t s) {
    epoll_ctl(p, EPOLL_CTL_DEL, s, NULL);
}

inline bool spoll_add(spoll_t p, socket_t s, void * u) {
    struct epoll_event event = {
        .events = EPOLLIN,
        .data = {
            .ptr = u,
        },
    };
    return epoll_ctl(p, EPOLL_CTL_ADD, s, &event);
}

inline int spoll_mod(spoll_t p, socket_t s, void * u, bool read, bool write) {
    struct epoll_event event = {
        .events = (read ? EPOLLIN : 0) | (write ? EPOLLOUT : 0),
        .data = {
            .ptr = u,
        },
    };
    return epoll_ctl(p, EPOLL_CTL_MOD, s, &event);
}

int spoll_wait(spoll_t p, spoll_event_t e) {
    struct epoll_event v[MAX_EVENT];
    
    int n = epoll_wait(p, v, sizeof v / sizeof *v, -1);

    for (int i = 0; i < n; ++i) {
        uint32_t flag = v[i].events;

        e[i].u = v[i].data.ptr;
        e[i].read = flag & EPOLLIN;
        e[i].write = flag & EPOLLOUT;
        e[i].error = flag & EPOLLERR;
        e[i].eof = flag & EPOLLHUP;
    }

    return n;
}

#endif/* __linux__ */
