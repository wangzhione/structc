#pragma once

#if defined(_WIN32) && defined(_MSC_VER)

#undef  FD_SETSIZE
#define FD_SETSIZE      (1024)

#include "spoll.h"

struct fds {
    void * u;
    bool write;
    socket_t fd;
};

struct spoll {
    fd_set fdr;
    fd_set fdw;
    fd_set fde;
    uint16_t len;
    struct fds s[FD_SETSIZE];
};

inline spoll_t spoll_create(void) {
    return calloc(1, sizeof(struct spoll));
}

inline bool spoll_invalid(spoll_t p) {
    return !p;
}

inline void spoll_delete(spoll_t p) {
    free(p);
}

void spoll_del(spoll_t p, socket_t s) {
    struct fds * begin = p->s, * end = p->s + p->len;
    while (begin < end) {
        if (begin->fd == s) {
            --p->len;
            while (begin < end) {
                begin[0] = begin[1];
                ++begin;
            }
            break;
        }
        ++begin;
    }
}

bool spoll_add(spoll_t p, socket_t s, void * u) {
    struct fds * begin, * end;
    if (p->len >= FD_SETSIZE)
        return true;

    begin = p->s;
    end = p->s + p->len;
    while (begin < end) {
        if (begin->fd == s)
            break;
        ++begin;
    }
    if (begin == end) {
        ++p->len;
        begin->fd = s;
    }
    begin->write = false;
    // 风险, u 用户数据赋值用户负责
    begin->u = u;
    return false;
}

void spoll_write(spoll_t p, socket_t s, void * u, bool enable) {
    struct fds * begin = p->s, * end = p->s + p->len;
    while (begin < end) {
        if (begin->fd == s) {
            begin->u = u;
            begin->write = enable;
            break;
        }
        ++begin;
    }
}

int spoll_wait(spoll_t p, spoll_event_t e) {
    socket_t fd;
    struct fds * s;
    int c, r, i, n, len = p->len;

    FD_ZERO(&p->fdr);
    FD_ZERO(&p->fdw);
    FD_ZERO(&p->fde);
    for (i = 0; i < len; ++i) {
        s = p->s + i;
        fd = s->fd;

        FD_SET(fd, &p->fdr);
        if (s->write)
            FD_SET(fd, &p->fdw);
        FD_SET(fd, &p->fde);
    }

    // wait for you ...
    n = select(0, &p->fdr, &p->fdw, &p->fde, NULL);
    if (n <= 0) RETURN(-1, "select n = %d error", n);

    for (c = i = 0; c < n && c < MAX_EVENT && i < len; ++i) {
        s = p->s + i;
        fd = s->fd;

        e[c].eof = false;
        e[c].read = FD_ISSET(fd, &p->fdr);
        e[c].write = s->write && FD_ISSET(fd, &p->fdw);

        r = true;
        if (FD_ISSET(fd, &p->fde)) {
            // 只要最后没有 error 那就 OK | 排除带外数据
            if (socket_get_error(fd))
                r = false;
        }

        // 保存最后错误信息
        if (e[c].read || e[c].write || !r) {
            e[c].error = !!r;
            e[c].u = s->u;
            ++c;
        }
    }

    return c;
}

#endif /* _WIN32 */
