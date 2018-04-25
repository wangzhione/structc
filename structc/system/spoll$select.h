#if defined(_WIN32)

#ifdef _MSC_VER
#undef  FD_SETSIZE
#define FD_SETSIZE      (1024)
#endif

#include <spoll.h>

struct trigger {
    void * ud;
    bool write;
    socket_t fd;
};

struct poll {
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    uint16_t len;
    struct trigger trgs[FD_SETSIZE];
};

//
// s_create     - 创建一个 poll 模型
// s_error      - true 表示当前创建的 poll 模型有问题
// s_delete     - 销毁一个创建的 poll 模型
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
// s_del        - 删除1检测的 socket
// s_add        - 添加监测的 socket, 并设置读模式, 失败返回 true
// s_write      - 修改当前 socket, 通过 enable = true 设置写模式
//
void s_del(poll_t p, socket_t s) {
    struct trigger * sr = p->trgs, * er = p->trgs + p->len;
    while (sr < er) {
        if (sr->fd == s) {
            --p->len;
            while (++sr < er)
                sr[-1] = sr[0];
            break;
        }
        ++sr;
    }
}

bool s_add(poll_t p, socket_t s, void * ud) {
    struct trigger * sr, * er;
#ifdef _MSC_VER
    if (p->len >= FD_SETSIZE || s <= 0)
        return true;
#else
    if (s >= FD_SETSIZE || s < 0)
        return false;
#endif

    sr = p->trgs;
    er = p->trgs + p->len;
    while (sr < er) {
        if (sr->fd == s)
            break;
        ++sr;
    }
    if (sr == er) {
        ++p->len;
        sr->fd = s;
    }
    sr->write = false;
    // 风险, ud 用户数据赋值用户负责
    sr->ud = ud;
    return false;
}

void s_write(poll_t p, socket_t s, void * ud, bool enable) {
    struct trigger * sr = p->trgs, * er = p->trgs + p->len;
    while (sr < er) {
        if (sr->fd == s) {
            sr->ud = ud;
            sr->write = enable;
            break;
        }
        ++sr;
    }
}

//
// s_wait       - wait 函数, 等待别人自投罗网
// p        : poll 模型
// e        : 返回的操作事件集
// return   : 返回待操作事件长度, < 0 表示失败
//
int s_wait(poll_t p, event_t e) {
    struct trigger * sr;
    socket_t fd, max = 0;
    socklen_t ren = sizeof(int);
    int c, r, i, n, len = p->len;

    FD_ZERO(&p->rfds);
    FD_ZERO(&p->wfds);
    FD_ZERO(&p->efds);
    for (i = 0; i < len; ++i) {
        sr = p->trgs + i;
        if ((fd = sr->fd) > max)
            max = fd;

        FD_SET(fd, &p->rfds);
        if (sr->write)
            FD_SET(fd, &p->wfds);
        FD_SET(fd, &p->efds);
    }

    // wait for you ...
    n = select((int)max + 1, &p->rfds, &p->wfds, &p->efds, NULL);
    if (n <= 0) RETURN(-1, "select n = %d error", n);

    for (c = i = 0; c < n && c < MAX_EVENT && i < len; ++i) {
        sr = p->trgs + i;
        fd = sr->fd;
        e[c].read = FD_ISSET(fd, &p->rfds);
        e[c].write = sr->write && FD_ISSET(fd, &p->wfds);

        r = 1;
        if (FD_ISSET(fd, &p->efds)) {
            // 只要最后没有 error 那就OK | 排除带外数据
            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&r, &ren) || r)
                r = 0;
        }

        // 保存最后错误信息
        if (e[c].read || e[c].write || !r) {
            e[c].error = !!r;
            e[c].ud = sr->ud;
            ++c;
        }
    }

    return c;
}

#endif
