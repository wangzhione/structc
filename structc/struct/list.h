#ifndef _H_LIST
#define _H_LIST

#include <struct.h>

//
// list.h 有序通用的单链表库
//

struct $list {
    struct $list * next;
};

#define LIST_HEAD struct $list $node

struct list { 
    struct $list * root;    // 存储链表的头节点

    icmp_f fadd;     // 链表中插入数据执行的方法
    icmp_f fget;     // 链表中查找数据执行的方法
    node_f fdie;     // 链表中删除数据执行的方法
};

typedef struct list * list_t;

//
// list_create_ - 构建 list 对象
// fadd     : 插入数据方法
// fget     : 获取数据方法
// fdie     : 销毁数据方法
// return   : 创建好的链表对象
//
inline list_t list_create_(icmp_f fadd, icmp_f fget, node_f fdie) {
    list_t list = malloc(sizeof(struct list));
    list->root = NULL;
    list->fadd = fadd;
    list->fget = fget;
    list->fdie = fdie;
    return list;
}

#define list_create(fadd, fget, fdie) \
list_create_((icmp_f)fadd, (icmp_f)fget, (node_f)fdie)

//
// list_delete - 链表数据销毁操作
// list     : 基础的链表结构
// return   : void
//
extern void list_delete(list_t list);

#endif//_H_LIST