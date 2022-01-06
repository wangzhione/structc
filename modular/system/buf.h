#pragma once

#include "msg.h"

#define MSG_BUF_OK      ( 0)
#define MSG_BUF_PARSE   (-7)    // 协议解析错误
#define MSG_BUF_SMALL   (-9)    // 内容过少, 协议还需要内容

//
// buffer recv send msg
//
typedef struct msg_buf * msg_buf_t;

//
// msg_buf_create - msg buffer 创建
// return   : msg buffer
//
extern msg_buf_t msg_buf_create(void);

//
// msg_buf_delete - msg buffer 删除
// q        : msg buffer
// return   : void
//
extern void msg_buf_delete(msg_buf_t q);

//
// buf_append - msg buffer 添加数据, 并尝试解析出结果
// q        : msg buffer
// p        : return msg
// data     : 内存数据
// sz       : 内存数据 size
// return   : MSG_BUF_PARSE 协议解析错误, MSG_BUF_SMALL 协议不完整
//
extern int msg_buf_append(msg_buf_t q,
                          const void * data, uint32_t sz,
                          msg_t * p);
