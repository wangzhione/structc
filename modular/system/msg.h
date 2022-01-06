﻿#pragma once

#include "struct.h"
#include "system.h"

//
// msg_t 网络传输协议结构
// sz -> type + len 本地小端字节序 -> data
//
typedef struct {
    // uint8_t type + uint24_t len + data[]
    uint32_t sz;
    char data[];
} * msg_t;

//
// MSG_TYPE - 得到当前消息 8bit 1 字节 type
// MSG_LEN  - 得到当前消息长度 24bit 3 字节 len
// MSG_SZ   - 8bit type + 24 bit len -> uint32_t sz
//
#define MSG_TYPE(sz)  (uint8_t)((uint32_t)(sz)>>24)
#define MSG_LEN( sz)  ((uint32_t)(sz)&0xFFFFFF)
#define MSG_SZ(t, n)  (((uint32_t)(uint8_t)(t)<<24)|(uint32_t)(n))

// small - 转本地字节序(小端)
inline uint32_t small(uint32_t x) {
# ifdef ISBIG
    uint8_t t;
    uint8_t * p = (uint8_t *)&x;

    t = p[0]; p[0] = p[3]; p[3] = t;
    t = p[1]; p[1] = p[2]; p[2] = t;
# endif
    return x;
}

//
// msg_create - msg 创建函数, send(fd, msg->data, msg->sz, 0)
// data     : 待发送的消息体
// len      : data 的长度
// return   : 创建好的 msg_t 消息体
//
inline static msg_t msg_create(const void * data, uint32_t len) {
    DCODE({
        if(!data || len <= 0 || len > 0xFFFFFF)
            EXIT("error data = %p, len = %u.\n", data, len);
    });

    uint32_t sz = len + sizeof(uint32_t);
    msg_t msg = malloc(sizeof(*msg) + sz);
    msg->sz = sz;
    
    // sz -> type + len 本地小端字节序 -> data
    sz = MSG_SZ(0, len);
    sz = small(sz);

    // 开始内存填充
    memcpy(msg->data, &sz, sizeof(uint32_t));
    memcpy(msg->data + sizeof(uint32_t), data, len);

    return msg;
}

//
// msg_delete - msg 删除函数
// msg      : msg_t 消息体
// return   : void
//
inline static void msg_delete(msg_t msg) {
    if (msg) free(msg);
}
