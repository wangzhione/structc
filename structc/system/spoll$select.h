#ifdef _WIN32

#ifdef _MSC_VER
#undef  FD_SETSIZE
#define FD_SETSIZE      (1024)
#endif

#include "spoll.h"

struct trigger {
    void * u;
    bool write;
    socket_t fd;
};

struct poll {
    fd_set fdr;
    fd_set fdw;
    fd_set fde;
    uint16_t len;
    struct trigger trg[FD_SETSIZE];
};

//
// s_create     - 创建 poll 对象
// s_error      - true 表示创建 poll 对象异常
// s_delete     - 销毁创建的 poll 对象
//
inline poll_t s_create(void) {
    return calloc(1, sizeof(struct poll));
}

inline bool s_error(poll_t p) {
    return NULL == p;
}

inline void s_delete(poll_t p) {
    free(p);
}

//
// s_del        - 删除监测的 socket
// s_add        - 添加监测的 socket, 并设置读模式, 失败返回 true
// s_write      - 修改监测的 socket, 通过 enable = true 设置写模式
//
void s_del(poll_t p, socket_t s) {
    struct trigger * begin = p->trg, * end = p->trg + p->len;
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

bool s_add(poll_t p, socket_t s, void * u) {
    struct trigger * begin, * end;
#ifdef _MSC_VER
    if (p->len >= FD_SETSIZE || s <= 0)
        return true;
#else
    if (p->len >= FD_SETSIZE || s < 0)
        return true;
#endif

    begin = p->trg;
    end = p->trg + p->len;
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

void s_write(poll_t p, socket_t s, void * u, bool enable) {
    struct trigger * begin = p->trg, * end = p->trg + p->len;
    while (begin < end) {
        if (begin->fd == s) {
            begin->u = u;
            begin->write = enable;
            break;
        }
        ++begin;
    }
}

//
// s_wait       - wait 函数, 守株待兔
// p        : poll 对象
// e        : 返回操作事件集
// return   : 返回操作事件长度, < 0 表示失败
//
int s_wait(poll_t p, event_t e) {
    struct trigger * g;
    socket_t fd, max = 0;
    socklen_t ren = sizeof(int);
    int c, r, i, n, len = p->len;

    FD_ZERO(&p->fdr);
    FD_ZERO(&p->fdw);
    FD_ZERO(&p->fde);
    for (i = 0; i < len; ++i) {
        g = p->trg + i;
        if ((fd = g->fd) > max)
            max = fd;

        FD_SET(fd, &p->fdr);
        if (g->write)
            FD_SET(fd, &p->fdw);
        FD_SET(fd, &p->fde);
    }

    // wait for you ...
    n = select((int)max + 1, &p->fdr, &p->fdw, &p->fde, NULL);
    if (n <= 0) RETURN(-1, "select n = %d error", n);

    for (c = i = 0; c < n && c < MAX_EVENT && i < len; ++i) {
        g = p->trg + i;
        fd = g->fd;
        e[c].eof = false;
        e[c].read = FD_ISSET(fd, &p->fdr);
        e[c].write = g->write && FD_ISSET(fd, &p->fdw);

        r = 1;
        if (FD_ISSET(fd, &p->fde)) {
            // 只要最后没有 error 那就 OK | 排除带外数据
            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&r, &ren) || r)
                r = 0;
        }

        // 保存最后错误信息
        if (e[c].read || e[c].write || !r) {
            e[c].error = !!r;
            e[c].u = g->u;
            ++c;
        }
    }

    return c;
}

#endif
