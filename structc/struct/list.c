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
}