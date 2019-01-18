#include "list.h"

//
// list_each - 链表循环处理, feach(x)
// list     : 链表对象
// feach    : node_f 节点遍历行为
// return   : void
//
void 
list_each(void * list, void * feach) {    
    if (list && feach) {
        struct $list * head = list;
        while (head) {
            struct $list * next = head->next;
            ((node_f)feach)(head);
            head = next;
        }
    }
}

//
// list_delete - 链表数据销毁操作, fdie(x)
// pist     : void ** 指向链表对象指针
// fdie     : node_f 链表中删除数据行为
// return   : void
//
void 
list_delete(void * pist, void * fdie) {
    if (pist && fdie) {
        // 详细处理链表数据变化
        struct $list * head = *(void **)pist;
        while (head) {
            struct $list * next = head->next;
            ((node_f)fdie)(head);
            head = next;
        }
        *(void **)pist = NULL;
    }
}

//
// list_next - 获取节点 n 的下一个节点
// n        : 当前节点
#undef  list_next
#define list_next(n) ((struct $list *)(n))->next

//
// list_add - 链表中添加数据, 升序 fadd(left, x) <= 0
// pist     : void ** 指向链表对象指针
// fadd     : cmp_f 链表中插入数据方法
// left     : 待插入的链表节点
// return   : void
//
void 
list_add(void * pist, void * fadd, void * left) {
    if (!pist || !fadd || !left)
        return;
    
    // 看是否是头节点
    struct $list * head = *(void **)pist;
    if (!head || ((cmp_f)fadd)(left, head) <= 0) {
        list_next(left) = head;
        *(void **)pist = left;
        return;
    }

    // 不是头节点, 挨个比对
    while (head->next) {
        if (((cmp_f)fadd)(left, head->next) <= 0)
            break;
        head = head->next;
    }

    // 添加最终的连接关系
    list_next(left) = head->next;
    head->next = left;
}

//
// list_get - 查找到链表中指定节点值, fget(left, x) == 0
// list     : 链表对象
// fget     : cmp_f 链表中查找数据行为
// left     : 待查找的辅助数据 
// return   : 查找到的节点, NULL 表示没有查到
//
void * 
list_get(void * list, void * fget, const void * left) {
    if (fget) {
        struct $list * head = list;
        while (head) {
            if (((cmp_f)fget)(left, head) == 0)
                return head;
            head = head->next;
        }
    }
    return NULL;
}

//
// list_pop - 弹出链表中指定节点值, fget(left, x) == 0
// pist     : void ** 指向链表对象指针
// fget     : cmp_f 链表中查找数据行为
// left     : 待查找的辅助数据 
// return   : 查找到的节点, NULL 表示没有查到 
//
void * 
list_pop(void * pist, void * fget, const void * left) {
    if (!pist || !*(void **)pist || !fget) 
        return NULL;

    // 看是否是头节点
    struct $list * head = *(void **)pist;
    if (((cmp_f)fget)(left, head) == 0) {
        *(void **)pist = head->next;
        return head;
    }

    // 不是头节点挨个处理
    for (struct $list * next; (next = head->next); head = next) {
        if (((cmp_f)fget)(left, next) == 0) {
            head->next = next->next;
            return next;
        }
    }

    return NULL;
}
