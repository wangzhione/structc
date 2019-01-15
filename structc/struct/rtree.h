#ifndef RTREE_H
#define RTREE_H

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

    cmp_f fcmp;
    new_f fnew;
    node_f fdie;

    cmp_f fget;
} * rtree_t;

//
// rtee_create - 创建一个红黑树结构
// fcmp     : 结点查找函数
// fnew     : 结点构造函数
// fdie     : 结点销毁函数
// return   : 返回构建红黑树
//
#define rtree_create(fcmp, fnew, fdie)                              \
rtree_create_((cmp_f)(fcmp), (new_f)(fnew), (node_f)(fdie))
extern rtree_t rtree_create_(cmp_f fcmp, new_f fnew, node_f fdie);

//
// rtree_delete - 红黑树销毁函数
// tree     : 待销毁的红黑树
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

#endif//RTREE_H
