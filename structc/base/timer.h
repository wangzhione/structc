#ifndef _TIMER_H
#define _TIMER_H

#include "atom.h"
#include "list.h"
#include "times.h"
#include "thread.h"

//
// timer_del - 删除定时器事件
// id       : 定时器 id
// return   : void
//
extern void timer_del(int id);

//
// timer_add - 添加定时器事件
// ms       : 执行间隔毫秒, <= 0 表示立即执行
// ftimer   : node_f 定时器行为
// arg      : 定时器参数
// return   : 定时器 id, < 0 标识 error
//
extern int timer_add(int ms, void * ftimer, void * arg);

#endif//_TIMER_H
