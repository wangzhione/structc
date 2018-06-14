#ifndef _H_LIST
#define _H_LIST

#include <struct.h>

//
// list.h 通用的单链表库
// void * list = NULL;
// 
struct $list {
    struct $list * next;
};

#define $LIST struct $list $node;

//
// list_next - 获取结点n的下一个结点.
// n        : 当前结点
//
#define list_next(n) ((void *)((struct $list *)(n))->next)

//
// list_delete - 链表数据销毁操作
// list     : 基础的链表结构
// pist     : 指向基础的链表结构
// fdie     : 链表中删除数据执行的方法
// return   : void
//
#define list_delete(list, fdie)                                         \
list_delete_((void **)&(list), (node_f)(fdie))
extern void list_delete_(void ** pist, node_f fdie);

//
// list_get - 匹配得到链表中指定值
// list     : 基础的链表结构
// fget     : 链表中查找数据执行的方法
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到
//
#define list_get(list, fget, left)                                      \
list_get_((list), (icmp_f)(fget), (const void *)(intptr_t)(left))
extern void * list_get_(void * list, icmp_f fget, const void * left);

//
// list_pop - 匹配弹出链表中指定值
// list     : 基础的链表结构
// pist     : 指向基础的链表结构
// fget     : 链表中查找数据执行的方法
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到 
//
#define list_pop(list, fget, left)                                      \
list_pop_((void **)&(list), (icmp_f)(fget), (const void *)(intptr_t)(left))
extern void * list_pop_(void ** pist, icmp_f fget, const void * left);

//
// list_add - 链表中添加数据, 从小到大 fadd(left, ) <= 0
// list     : 基础的链表结构
// pist     : 指向基础的链表结构
// fadd     : 插入数据方法
// left     : 待插入的链表结点
// return   : void
//
#define list_add(list, fadd, left)                                      \
list_add_((void **)&(list), (icmp_f)(fadd), (void *)(intptr_t)(left))
extern void list_add_(void ** pist, icmp_f fadd, void * left);

//
// list_each - 链表循环处理函数, 仅仅测试而已
// list     : 基础的链表结构
// feach    : 处理每个结点行为函数
// return   : void
//
#define list_each(list, feach)                                          \
list_each_((list), (node_f)(feach))
extern void list_each_(void * list, node_f feach);

#endif//_H_LIST
