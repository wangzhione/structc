#ifndef _H_LIST
#define _H_LIST

#include <struct.h>

//
// list.h 有序通用的单链表库
//

struct $list {
    struct $list * next;
};

#define $LIST struct $list $node;

struct list { 
    struct $list * root;    // 存储链表的头节点

    icmp_f fadd;     // 链表中插入数据执行的方法
    icmp_f fget;     // 链表中查找数据执行的方法
    node_f fdie;     // 链表中删除数据执行的方法
};

typedef struct list * list_t;

//
// list_next - 获取结点n的下一个结点.
// n		: 当前结点
//
#define list_next(n) ((void *)((struct $list *)(n))->next)

//
// list_create_ - 构建 list 对象
// fadd     : 插入数据方法
// fget     : 获取数据方法
// return   : 创建好的链表对象
//
inline list_t list_create_(icmp_f fadd, icmp_f fget) {
    list_t list = malloc(sizeof(struct list));
    list->root = NULL;
    list->fadd = fadd;
    list->fget = fget;
    list->fdie = NULL;
    return list;
}

#define list_create(fadd, fget) \
list_create_((icmp_f)fadd, (icmp_f)fget)

//
// list_delete - 链表数据销毁操作
// list     : 基础的链表结构
// return   : void
//
extern void list_delete(list_t list);

//
// list_get - 匹配得到链表中指定值
// list     : 基础的链表结构
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到
//
extern void * list_get_(list_t list, const void * left);
#define list_get(list, left) \
list_get_(list, (const void *)(intptr_t)left)

//
// list_pop - 匹配弹出链表中指定值
// list     : 基础的链表结构
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到 
//
extern void * list_pop_(list_t list, const void * left);
#define list_pop(list, left) \
list_pop_(list, (const void *)(intptr_t)left)

//
// list_add - 链表中添加数据, 从小到大 fadd(left, ) <= 0
// list     : 基础的链表结构 
// left     : 待插入的链表结点
// return   : void
// 
extern void list_add(list_t list, void * left);

//
// list_each - 链表循环处理函数, 仅仅测试而已
// list     : 基础的链表结构
// feach    : 处理每个结点行为函数
// return   : void
//
extern void list_each_(list_t list, node_f feach);
#define list_each(list, feach) \
list_each_(list, (node_f)feach)

#endif//_H_LIST