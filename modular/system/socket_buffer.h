#pragma once

#include "struct.h"

#define SOCKET_BUFFER_MEMORY            0
#define SOCKET_BUFFER_OBJECT            1
#define SOCKET_BUFFER_RAWPOINTER        2

#define SOCKET_SENDBUFFER_USEROBJECT    ((size_t)-1)

struct socket_sendbuffer {
    int id;
    int type;
    const void * buffer;
    size_t sz;
};

static inline const void * clone_buffer(struct socket_sendbuffer * buf, size_t * sz) {
    switch (buf->type) {
    case SOCKET_BUFFER_MEMORY:
        *sz = buf->sz;
        return buf->buffer;
    case SOCKET_BUFFER_OBJECT:
        *sz = SOCKET_SENDBUFFER_USEROBJECT;
        return buf->buffer;
    case SOCKET_BUFFER_RAWPOINTER:
        // It's a raw pointer, we need make a copy
        *sz = buf->sz;
        void * copy = malloc(*sz);
        memcpy(copy, buf->buffer, *sz);
        return copy;
    default:
        PERR("{%d, %d, %p, %zd}", buf->id, buf->type, buf->buffer, buf->sz);
        // never get here
        *sz = 0;
        return NULL;
    }
}
