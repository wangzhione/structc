#ifndef _H_CJSON
#define _H_CJSON

#include <tstr.h>

//
// c json fast parse, type is design
//

#ifndef _STRUCT_JSON

#define JSON_NULL      (0u << 0)
#define JSON_TRUE      (1u << 0)
#define JSON_FALSE     (1u << 1)
#define JSON_NUMBER    (1u << 2)
#define JSON_STRING    (1u << 3)
#define JSON_OBJECT    (1u << 4)
#define JSON_ARRAY     (1u << 5)

struct json {
    unsigned char type;         // CJSON_NULL - JSON_ARRAY
    struct json * next;         // type = OBJECT or ARRAY 结点采用链表链接
    struct json * chid;         // type = OBJECT or ARRAY 时候 chid 结点就有数据

    char * keys;                // json 结点的 key 名称串
    union {
        char * vals;            // type == STRING 就是个 字符串
        double vald;            // type == NUMBER 就是个 number
    };
};

//
// 定义 json 对象类型
//
typedef struct json * json_t;

//
// json_vali - 得到结点的 int 值
// item     : 处理的结点
//          : 返回 number int 值
//
#define json_vali(item) ((int)(item)->vald)

#define _STRUCT_JSON
#endif//_STRUCT_JSON

//
// json_delete - json 对象销毁
// c        : json 对象
// return   : void
//
extern void json_delete(json_t c);

#endif//_H_CJSON
