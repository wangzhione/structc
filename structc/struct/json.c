#include <json.h>

//
// json_delete - json 对象销毁
// c        : json 对象
// return   : void
//
void 
json_delete(json_t c) {
    while (c) {
        json_t next = c->next;
        free(c->keys);
        if (c->type & JSON_STRING)
            free(c->vals);

        // 子结点 继续递归删除
        if (c->chid)
            json_delete(c->chid);

        c = next;
    }
}


//-------------------------------------json parse begin-----------------------------------

//-------------------------------------json parse end-------------------------------------

//-------------------------------------json print begin-----------------------------------

//-------------------------------------json print end-------------------------------------