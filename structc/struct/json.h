#ifndef _JSON_H
#define _JSON_H

#include "tstr.h"
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>

//
// c json fast parse, type is all design
//
#ifndef JSON_NULL

#define JSON_NULL           (0u << 0)
#define JSON_BOOL           (1u << 1)
#define JSON_NUMBER         (1u << 2)
#define JSON_STRING         (1u << 3)
#define JSON_OBJECT         (1u << 4)
#define JSON_ARRAY          (1u << 5)
#define JSON_CONST          (1u << 6)

struct json {
    unsigned char type;     // CJSON_NULL - JSON_ARRAY and JSON_CONST
    struct json * next;     // type & OBJECT or ARRAY -> 下个节点链表
    struct json * chid;     // type & OBJECT or ARRAY -> 对象节点数据

    char * key;             // json 节点的 key
    union {
        char * str;         // type & STRING -> 字符串
        double num;         // type & NUMBER -> number
    };
};

// 定义 json 对象类型
//
typedef struct json * json_t;

//
// json_int - 得到节点的 int 值
// item     : json 节点
//          : 返回 number int 值
//
#define json_int(item) ((int)(item)->num)

#endif//JSON_NULL

// json_str - json 字符串分离, 需要自行 free
inline char * json_str(json_t item) {
    item->type &= JSON_CONST;
    return item->str;
}

//
// json_delete - json 对象销毁
// c        : json 对象
// return   : void
//
extern void json_delete(json_t c);

//
// json_len - 获取 json 对象长度
// c        : json 对象
// return   : 返回 json 对象长度
//
extern int json_len(json_t c);

//
// json_array - 通过索引获取 json 数组中子节点
// arr      : json 数组
// i        : [0, json_len()) 索引
// return   : 返回对应的数组节点
//
extern json_t json_array(json_t arr, int i);

//
// json_object - 获取 json 对象中子对象 
// obj      : json 对象
// k        : key
// return   : 返回对应的对象节点
//
extern json_t json_object(json_t obj, const char * k);

//----------------------------------json parse begin--------------------------------

//
// json parse low level api
// 

// json_mini - json 清洗函数
size_t json_mini(char * str);
// json_parse - json 解析函数
json_t json_parse(const char * str);

//
// json_file - 通过文件构造 json 对象
// json_create  - 通过字符串构造 json 对象
// str      : 字符串
// path     : 文件路径
// return   : json_t 对象
//
extern json_t json_file(const char * path);
extern json_t json_create(const char * str);

//----------------------------------json parse end----------------------------------

//----------------------------------json print begin--------------------------------

//
// json_print - 获取 json 对象的打印字符串
// c        : json_t 对象
// return   : 返回生成的 json 字符串, 需要自行 free
//
extern char * json_print(json_t c);

//----------------------------------json print end----------------------------------

//----------------------------------json utils begin--------------------------------

// json_new - 构造 json 对象
inline json_t json_new(void) {
    return calloc(1, sizeof(struct json));
}

//
// json_new_xxxx - 创建对应的对象
// t        : 创建对象类型
// v        : 具体的值
// return   : json_t 创建好的对象
//
inline json_t json_new_type(unsigned char t) {
    json_t item = json_new();
    item->type = t;
    return item;
}

inline json_t json_new_null(void) {
    return json_new();
}

inline json_t json_new_bool(bool v) {
    json_t item = json_new_type(JSON_BOOL);
    item->num = v ;
    return item;
}

inline json_t json_new_number(double v) {
    json_t item = json_new_type(JSON_NUMBER);
    item->num = v;
    return item;
}

inline json_t json_new_string(const char * v) {
    json_t item = json_new_type(JSON_STRING);
    item->str = strdup(v);
    return item;
}

inline json_t json_new_object(void) {
    return json_new_type(JSON_OBJECT);
}

inline json_t json_new_array(void) {
    return json_new_type(JSON_ARRAY);
}

//
// json_new_arrays - 创建数组类型 json 对象
// t        : 类型宏
// arr      : 原数组对象
// n        : 原数组长度
// return   : 返回创建好的 json 数组
//
extern json_t json_new_arrays(unsigned char t, const void * arr, int n);

//
// json_detach_xxxxx - 通过索引分离出 json 子对象
// arr      : json_t 数组
// i        : [0, len()) 索引
// obj      : json_t 对象
// k        : key
// return   : 分离出的 json 对象
//
extern json_t json_detach_array(json_t arr, int i);
extern json_t json_detach_object(json_t obj, const char * k);

//----------------------------------json utils end----------------------------------

#endif//_JSON_H
