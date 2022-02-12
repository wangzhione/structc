#pragma once

#include "struct.h"

#define SOCKET_INFO_UNKNOWN 0
#define SOCKET_INFO_LISTEN  1
#define SOCKET_INFO_TCP     2
#define SOCKET_INFO_UDP     3
#define SOCKET_INFO_BIND    4
#define SOCKET_INFO_CLOSING 5

struct socket_info {
    int id;
    int type;
    uint64_t opaque;
    uint64_t read;
    uint64_t write;
    uint64_t rtime;
    uint64_t wtime;
    int64_t wbuffer;
    uint8_t reading;
    uint8_t writing;
    char name[128];
    struct socket_info * next;
};

static inline struct socket_info * socket_info_create(struct socket_info * last) {
    struct socket_info * si = malloc(sizeof(struct socket_info));
    memset(si, 0, sizeof(struct socket_info));
    si->next = last;
    return si;
}

static inline void socket_info_release(struct socket_info * si) {
    while (si) {
        struct socket_info * temp = si;
        si = si->next;
        free(temp);
    }
}
