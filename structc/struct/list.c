#include <list.h>

//
// list_delete - 链表数据销毁操作
// list     : 基础的链表结构
// return   : void
//
void 
list_delete(list_t list) {
    struct $list * head;
    if (!list || !list->fdie)
        return;

    // 详细处理链表数据变化
    head = list->root;
    while (head) {
        struct $list * next = head->next;
        list->fdie(head);
        head = next;
    }
    
    free(list);
}

//
// list_get - 匹配得到链表中指定值
// list     : 基础的链表结构
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到
//
void * 
list_get_(list_t list, const void * left) {
    struct $list * head;
    if (!list || !list->fget)
        return NULL;
    
    head = list->root;
    while (head) {
        if (list->fget(left, head) == 0)
            return head;
        head = head->next;
    }

    return NULL;
}

//
// list_pop - 匹配弹出链表中指定值
// list     : 基础的链表结构
// left     : 待查找的结点内容 
// return   : 查找到的节点, NULL 表示没有查到 
//
void * 
list_pop_(list_t list, const void * left) {
    struct $list * head, * next;
    if (!list || !list->fget)
        return NULL;

    // 看是否是头节点
    head = list->root;
    if (list->fget(left, head) == 0) {
        list->root = head->next;
        return head;
    }

    // 不是头节点挨个处理
    while (!!(next = head->next)) {
        if (list->fget(left, next) == 0) {
            head->next = next->next;
            return next;
        }
        head = next;
    }

    return NULL;
}

//
// list_add - 链表中添加数据
// list     : 基础的链表结构 
// left     : 待插入的链表结点
// return   : void
// 
void 
list_add(list_t list, void * left) {
    struct $list * head;
    if (!list || !list->fadd || !left)
        return;
    
    // 看是否是头结点
    head = list->root;
    if (!head || list->fadd(left, head) <= 0) {
        ((struct $list *)left)->next = list->root;
        list->root = left;
        return;
    }

    // 不是头节点, 挨个比对
    while (head->next) {
        if (list->fadd(left, head->next) <= 0)
            break;
        head = head->next;
    }

    // 添加最终的连接关系
    ((struct $list *)left)->next = head->next;
    head->next = left;
}

//
// list_each - 链表循环处理函数, 仅仅测试而已
// list     : 基础的链表结构
// feach    : 处理每个结点行为函数
// return   : void
//
void 
list_each_(list_t list, node_f feach) {    
    if (list) {
        struct $list * head = list->root;
        while (head) {
            feach(head);
            head = head->next;
        }
    }
}
