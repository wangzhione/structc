#ifndef _RSQ_H
#define _RSQ_H

#include "msg.h"
#include "atom.h"

//
// recv send msg q
//
typedef struct rsq * rsq_t;

//
// rsq_create - msg q 创建
// limit    : -1 不限制容量 0 默认容量 > 0 指定容量
// return   : msg q
//
extern rsq_t rsq_create(int limit);

//
// rsq_delete - msg q 删除
// q        : msg q
// return   : void
//
extern void rsq_delete(rsq_t q);

//
// rsq_pop  - q pop msg
// q        : msg q
// psg      : return msg
// return   : EParse 协议解析错误, EBig 包过大, ESmall 包过小
//
extern int rsq_pop(rsq_t q, msg_t * psg);

//
// rsq_push - q push msg
// q        : msg q
// data     : 内存数据
// sz       : 内存数据 size
// return   : void
//
extern void rsq_push(rsq_t q, const void * data, uint32_t sz);

#endif//_RSQ_H

