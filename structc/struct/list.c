#include "list.h"

//
// list_delete - 链表数据销毁操作
// pist     : 指向基础的链表结构
// fdie     : 链表中删除数据执行的方法
// return   : void
//
void 
list_delete_(void ** pist, node_f fdie) {
    if (pist && fdie) {
        // 详细处理链表数据变化
        struct $list * head = *pist;
        while (head) {
            struct $list * next = head->next;
            fdie(head);
            head = next;
        }
        *pist = NULL;
    }
}

//
// list_get - 匹配得到链表中指定值
// list     : 基础的链表结构
// fget     : 链表中查找数据执行的方法
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到
//
void * 
list_get_(void * list, icmp_f fget, const void * left) {
    if (fget) {
        struct $list * head = list;
        while (head) {
            if (fget(left, head) == 0)
                return head;
            head = head->next;
        }
    }
    return NULL;
}

//
// list_pop - 匹配弹出链表中指定值
// pist     : 指向基础的链表结构
// fget     : 链表中查找数据执行的方法
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到 
//
void * 
list_pop_(void ** pist, icmp_f fget, const void * left) {
    struct $list * head, * next;
    if (!pist || fget)
        return NULL;

    // 看是否是头节点
    head = *pist;
    if (fget(left, head) == 0) {
        *pist = head->next;
        return head;
    }

    // 不是头节点挨个处理
    while (!!(next = head->next)) {
        if (fget(left, next) == 0) {
            head->next = next->next;
            return next;
        }
        head = next;
    }

    return NULL;
}

//
// list_next - 获取结点n的下一个结点.
// n        : 当前结点
//
#undef  list_next
#define list_next(n) ((struct $list *)(n))->next

//
// list_add - 链表中添加数据, 从小到大 fadd(left, ) <= 0
// pist     : 指向基础的链表结构
// fadd     : 插入数据方法
// left     : 待插入的链表结点
// return   : void
//
void 
list_add_(void ** pist, icmp_f fadd, void * left) {
    struct $list * head;
    if (!pist || !fadd || !left)
        return;
    
    // 看是否是头结点
    head = *pist;
    if (!head || fadd(left, head) <= 0) {
        list_next(left) = head;
        *pist = left;
        return;
    }

    // 不是头节点, 挨个比对
    while (head->next) {
        if (fadd(left, head->next) <= 0)
            break;
        head = head->next;
    }

    // 添加最终的连接关系
    list_next(left) = head->next;
    head->next = left;
}

//
// list_each - 链表循环处理函数, 仅仅测试而已
// list     : 基础的链表结构
// feach    : 处理每个结点行为函数
// return   : void
//
void 
list_each_(void * list, node_f feach) {    
    if (list && feach) {
        struct $list * head = list;
        while (head) {
            struct $list * next = head->next;
            feach(head);
            head = next;
        }
    }
}
