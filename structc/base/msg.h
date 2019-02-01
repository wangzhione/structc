#ifndef _MSG_H
#define _MSG_H

#include "struct.h"

#ifdef _MSC_VER
//
// CPU 检测 x64 or x86
// ISX64 defined 表示 x64 否则 x86
//
#  if defined(_M_ARM64) || defined(_M_X64)
#    define ISX64
#  endif
//
// _M_PPC 为 PowerPC 平台定义, 现在已不支持
//
#  if defined(_M_PPC)
#    define ISBIG
#  endif
#else
#  if defined(__x86_64__)
#    define ISX64
#  endif
//
// 大小端检测 : ISBENIAN defined 表示大端
//
#  if defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN_BITFIELD)
#    define ISBIG
#  endif
#endif

// hton - 本地字节序转网络字节序(大端)
inline uint32_t hton(uint32_t x) {
#  ifdef ISBIG
    return x;
#  else
    uint8_t t;
    union { uint32_t i; uint8_t s[sizeof(uint32_t)]; }u = { x };
    t = u.s[0]; u.s[0] = u.s[sizeof(u)-1]; u.s[sizeof(u)-1] = t;
    t = u.s[1]; u.s[1] = u.s[sizeof(u)-2]; u.s[sizeof(u)-2] = t;
    return u.i;
#  endif
}

// noth - 网络字节序(大端)转本地字节序
inline uint32_t ntoh(uint32_t x) {
    return hton(x);
}

//
// msg_t 网络传输协议结构
// sz -> type + len 网络字节序 -> data
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
    
    // type + len -> 协议值 -> 网络传输约定值
    sz = MSG_SZ(0, len);
    sz = hton(sz);

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

#endif//_MSG_H
