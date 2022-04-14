﻿#pragma once

#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>

#include "chars.h"
#include "strext.h"

//
// cj json fast parse, type is all design
// https://www.json.org/json-zh.html
//
#ifndef JSON_NULL

#define JSON_NULL           (0u << 0)
#define JSON_TRUE           (1u << 1)
#define JSON_FALSE          (1u << 2)
#define JSON_NUMBER         (1u << 3)
#define JSON_STRING         (1u << 4)
#define JSON_OBJECT         (1u << 5)
#define JSON_ARRAY          (1u << 6)
#define JSON_CONST          (1u << 7)

// JSON_OBJECT or JSON_ARRAY data struct
// |
// child
//     -> next -> ... -> next 
//                          |
//                          child
//                              -> next -> ..
struct json {
    unsigned type;          // C JSON_NULL - JSON_ARRAY and JSON_CONST
    struct json * next;     // type & JSON_ARRAY | JSON_OBJECT -> 同级下个结点
    struct json * child;    // type & JSON_ARRAY | JSON_OBJECT -> 子结点

    char * key;             // json 结点的 key
    union {
        int len;            // type & JSON_ARRAY | JSON_OBJECT is json child len
        char * str;         // type & JSON_STRING is 字符串
        double num;         // type & JSON_NUMBER is number
    };
};

// 定义 json 对象类型
//
typedef struct json * json_t;

// json_int - 得到结点的 int 值. 
// double 10 他可以表示十进制的15或16位有效数字.
// int 范围 [-2^63, 2^63-1] 即 -2,147,483,648 到 2,147,483,647 约 9 到 10 位
#define json_int(item) ((int)(item)->num)

#endif//JSON_NULL

//
// json_delete - json 对象销毁
// cj        : json 对象
// return   : void
//
extern void json_delete(json_t cj);

//
// json_len - 获取 json 对象长度
// cj        : json 对象
// return   : 返回 json 对象长度
//
extern int json_len(json_t cj);

//
// json_array - 通过索引获取 json 数组中子结点
// aj       : json 数组
// i        : [0, json_len()) 索引
// return   : 返回对应的数组结点
//
extern json_t json_array(json_t aj, int i);

//
// json_object - 获取 json 对象中子对象 
// obj      : json 对象
// k        : key
// return   : 返回对应的对象结点
//
extern json_t json_object(json_t obj, const char * k);

// json_mini - json 清洗函数 low level api
size_t json_mini(char * str);
// json_parse - json 解析函数 low level api
json_t json_parse(const char * str);

// json_create  解析字符串构造 json 对象
extern json_t json_create(const char * str);
// json_file 通过文件 path 解析文件内容构造 json 对象
extern json_t json_file(const char * path);

// json_detach_str - json 字符串分离, 需要自行 free
inline char * json_detach_str(json_t item) {
    item->type &= JSON_CONST;
    return item->str;
}

// json_detach_array - i ∈ [0, len()) 索引, 分离出 json 子对象
extern json_t json_detach_array(json_t aj, int i);
extern json_t json_detach_object(json_t obj, const char * k);

//
// json_string - 生成 json 对象 char * 字符串
// cj        : json_t 对象
// return   : 返回生成的 json 字符串, 需要自行 free
//
extern char * json_string(json_t cj);

// json_new - 构造 json 对象 (json null node)
inline json_t json_new(void) {
    return calloc(1, sizeof(struct json));
}

//
// json_new_xxxx - 创建对应的对象
// type     : 创建对象 JSON define 类型宏
// v        : 具体的值
// return   : json_t 创建好的对象
//
inline json_t json_new_type(unsigned type) {
    json_t item = json_new();
    item->type = type;
    return item;
}

inline json_t json_new_bool(bool v) {
    json_t item;
    item = v ? json_new_type(JSON_TRUE) : json_new_type(JSON_FALSE);
    item->num = v;
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
// json_create_array - 创建数组类型 json 对象
// type     : 创建对象 JSON define 类型宏
// a        : 原数组对象
// n        : 原数组长度
// return   : 返回创建好的 json 数组
//
extern json_t json_create_array(unsigned type, const void * a, int n);
