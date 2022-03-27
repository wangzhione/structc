#pragma once

#if defined(_WIN32) && defined(_MSC_VER)

#undef  FD_SETSIZE
#define FD_SETSIZE      (1024)

#include "spoll.h"

struct fds {
    void * u;
    SOCKET fd;
    bool read;
    bool write;
};

struct spoll {
    // fdr 调用 select 的 read 监听参数, rset 原始 fd_set 集合
    fd_set fdr, rset;
    fd_set fdw, wset;
    fd_set fde, eset;
    uint16_t len;
    struct fds s[FD_SETSIZE];
#ifndef _WIN32
    SOCKET maxfd;
#endif
};

inline spoll_t spoll_create(void) {
    return calloc(1, sizeof(struct spoll));
}

inline bool spoll_invalid(spoll_t p) {
    return p == NULL;
}

inline void spoll_delete(spoll_t p) {
    free(p);
}

void spoll_del(spoll_t p, SOCKET s) {
    struct fds * begin = p->s, * end = p->s+p->len;
    while (begin < end && begin->fd != s) {
        ++begin;
    }
    // 需要一定能找到要删除 SOCKET
    assert(begin < end);

    // fset clear fd
    if (begin->read)
        FD_CLR(s, &p->rset);
    if (begin->write)
        FD_CLR(s, &p->wset);
    FD_CLR(s, &p->eset);

    // clear begin, 这里使用简单方式, 也可以走状态机不用数据整理
    --p->len;
    while (begin < end) {
        begin[0] = begin[1];
        ++begin;
    }

#ifndef _WIN32
    // 非 window select 依赖 max fd
    if (s == p->maxfd) {
        p->maxfd = 0;
        for (begin = p->s, end = p->s+p->len; begin < end; begin++) {
            if (p->maxfd < begin->fd)
                p->maxfd = begin->fd;
        }
    }
#endif
}

bool spoll_add(spoll_t p, SOCKET s, void * u) {
    if (p->len >= FD_SETSIZE) {
        RETURN(true, "too many fd len = %d", p->len);
    }

    struct fds * begin = p->s, * end = p->s+p->len;
    while (begin < end && begin->fd != s) {
        ++begin;
    }
    // 默认必须是添加, 而不是修改
    assert(begin == end);

    ++p->len;

    begin->fd = s;
    // 风险点, u 用户数据赋值用户负责
    begin->u = u;
    begin->read = true;
    begin->write = false;

    FD_SET(s, &p->rset);
    FD_SET(s, &p->eset);

#ifndef _WIN32
    if (p->maxfd < s)
        p->maxfd = s;
#endif

    return false;
}

int spoll_mod(spoll_t p, SOCKET s, void * u, bool read, bool write) {
    struct fds * begin = p->s, * end = p->s+p->len;
    while (begin < end) {
        if (begin->fd == s) {
            begin->u = u;

            if (begin->read != read) {
                begin->read = read;
                if (read) {
                    FD_SET(s, &p->rset);
                } else {
                    FD_CLR(s, &p->rset);
                }
            }

            if (begin->write != write) {
                begin->write = write;
                if (read) {
                    FD_SET(s, &p->wset);
                } else {
                    FD_CLR(s, &p->wset);
                }
            }

            break;
        }
        ++begin;
    }
    return 0;
}

int spoll_wait(spoll_t p, spoll_event_t e) {
    // 复制构造 readfds writefds exceptfds
    memcpy(&p->fdr, &p->rset, sizeof(fd_set));
    memcpy(&p->fdw, &p->wset, sizeof(fd_set));
    memcpy(&p->fde, &p->eset, sizeof(fd_set));

    // wait for you ...
#ifndef _WIN32
    int n = select(p->maxfd+1, &p->fdr, &p->fdw, &p->fde, NULL);
#else
    int n = select(0, &p->fdr, &p->fdw, &p->fde, NULL);
#endif
    if (n <= 0) RETURN(-1, "select n = %d error", n);
    if (n > MAX_EVENT) {
        POUT("select too much n = %d ", n);
        n = MAX_EVENT;
    }
    for (int j = 0, i = 0; j < n && i < p->len; ++i) {
        struct fds * s = p->s+i;
        SOCKET fd = s->fd;

        e[j].read = s->read && FD_ISSET(fd, &p->fdr);
        e[j].write = s->write && FD_ISSET(fd, &p->fdw);
        // 只要最后没有 error 那就 OK | 排除带外数据
        e[j].error = FD_ISSET(fd, &p->fde) && socket_get_error(fd) == 0;

        // 看是否需要保存最后错误信息
        if (e[j].read || e[j].write || e[j].error) {
            e[j].eof = false;
            e[j].u = s->u;
            ++j;
        }
    }

    return n;
}

#endif /* _WIN32 */
