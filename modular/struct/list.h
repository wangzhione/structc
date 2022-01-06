#pragma once

#include "struct.h"

//
// list.h 似魔鬼的步伐, 单链表库
// $LIST 需要嵌入 struct 的第一行
// void * list = NULL;      //        create list
// list_delete(list, fide); // [可选] delete list
//
struct $list {
    struct $list * next;
};

#define $LIST struct $list $node;

//
// list_next - 获取结点 n 的下一个结点
// n        : 当前结点
#define list_next(n) ((void *)((struct $list *)(n))->next)

//
// list_each - 链表循环处理, feach(x)
// list     : 链表对象
// feach    : node_f 结点遍历行为
// return   : void
//
extern void list_each(void * list, void * feach);

//
// list_delete - 链表数据销毁操作, fdie(x)
// pist     : void ** 指向链表对象指针
// fdie     : node_f 链表中删除数据行为
// return   : void
//
extern void list_delete(void * pist, void * fdie);

//
// list_add - 链表中添加数据, 升序 fadd(left, x) <= 0
// pist     : void ** 指向链表对象指针
// fadd     : cmp_f 链表中插入数据方法
// left     : 待插入的链表结点
// return   : void
//
extern void list_add(void * pist, void * fadd, void * left);

//
// list_get - 查找到链表中指定结点值, fget(left, x) == 0
// list     : 链表对象
// fget     : cmp_f 链表中查找数据行为
// left     : 待查找的辅助数据 
// return   : 查找到的结点, NULL 表示没有查到
//
extern void * list_get(void * list, void * fget, const void * left);

//
// list_pop - 弹出链表中指定结点值, fget(left, x) == 0
// pist     : void ** 指向链表对象指针
// fget     : cmp_f 链表中查找数据行为
// left     : 待查找的辅助数据 
// return   : 查找到的结点, NULL 表示没有查到 
//
extern void * list_pop(void * pist, void * fget, const void * left);
