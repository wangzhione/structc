#ifndef _H_MQ
#define _H_MQ

#include <struct.h>

typedef struct mq * mq_t;

//
// mq_create - 消息队列创建
// return   : 创建好的消息队列
//
extern mq_t mq_create(void);

//
// mq_delete - 消息队列销毁
// q        : 消息队列对象
// fdie     : 删除 push 进来的结点
// return   : void
//
extern void mq_delete(mq_t q, node_f fdie);

//
// mq_push - 消息队列中压入数据
// q        : 消息队列对象
// m        : 压入的消息
// return   : void
// 
extern void mq_push(mq_t q, void * m);

//
// mq_pop - 消息队列中弹出消息,并返回数据
// q        : 消息队列对象
// return   : 返回队列尾巴, 队列 empty时候, 返回NULL
//
extern void * mq_pop(mq_t q);

//
// mq_len - 消息队列的长度
// q        : 消息队列对象
// return   : 返回消息队列长度
// 
extern int mq_len(mq_t q);

#endif//_H_MQ
