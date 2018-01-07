#ifndef _H_RTREE
#define _H_RTREE

#include <struct.h>

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

    icmp_f fcmp;
    vnew_f fnew;
    node_f fdie;
} * rtree_t;

//
// rtee_create - 创建一个红黑树结构
// fcmp     : 结点查找函数
// fnew     : 结点构造函数
// fdie     : 结点销毁函数
// return   : 返回构建红黑树
//
#define rtee_create(fcmp, fnew, fdie) \
rtree_create_((icmp_f)(fcmp), (vnew_f)(fnew), (node_f)(fdie))

extern rtree_t rtree_create_(icmp_f fcmp, vnew_f fnew, node_f fdie);

//
// rtree_delete - 红黑树销毁函数
// tree     : 待销毁的红黑树
// return   : void
//
extern void rtree_delete(rtree_t tree);

#endif//_H_RTREE