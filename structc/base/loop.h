#ifndef _LOOP_H
#define _LOOP_H

#include "q.h"
#include "atom.h"
#include "thread.h"

typedef struct loop * loop_t;

//
// loop_delete - 轮询对象销毁
// p        : 轮询对象
// return   : void
//
extern void loop_delete(loop_t p);

//
// loop_push  - 消息压入轮询器
// p        : 轮询对象
// m        : 消息
// return   : void
//
extern void loop_push(loop_t p, void * m);

//
// loop_create - 创建轮询对象
// frun     : node_f 消息处理行为
// fdie     : node_f 消息销毁行为
// return   : 轮询对象
//
extern loop_t loop_create(void * frun, void * fdie);

#endif//_LOOP_H
