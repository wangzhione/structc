#ifndef _RTREE_H
#define _RTREE_H

#include "struct.h"

//
// 红黑树通用结构, 需要将 $RTREE 放在结构开头部位
//

struct $rtree {
    uintptr_t parentc;
    struct $rtree * left;
    struct $rtree * right;
};

#define $RTREE struct $rtree $node;

typedef struct {
    struct $rtree * root;
    cmp_f fget; // cmp_f 节点查找时比较行为
    cmp_f fcmp;
    new_f fnew;
    node_f fdie;
} * rtree_t;

//
// rtee_create - 创建一个红黑树对象
// fcmp     : cmp_f 节点插入时比较行为
// fnew     : new_f 节点插入时构造行为
// fdie     : node_f 节点删除时销毁行为
// return   : 返回构建红黑树对象
//
extern rtree_t rtree_create(void * fcmp, void * fnew, void * fdie);

//
// rtree_delete - 红黑树删除函数
// tree     : 待删除的红黑树
// return   : void
//
extern void rtree_delete(rtree_t tree);

//
// rtree_search - 红黑树查找函数
// tree     : 待查找的红黑树结构
// return   : 返回查找的节点
//
extern void * rtree_search(rtree_t tree, void * pack);

//
// rtree_insert - 红黑树中插入节点 fnew(pack)
// tree     : 红黑树结构
// pack     : 待插入基础结构
// return   : void
//
extern void rtree_insert(rtree_t tree, void * pack);

//
// rtree_remove - 红黑树中删除节点
// tree     : 红黑树结构
// pack     : 待删除基础结构
// return   : void
//
extern void rtree_remove(rtree_t tree, void * pack);

#endif//_RTREE_H
