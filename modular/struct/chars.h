#pragma once

#include "struct.h"

// 轻量 char * 串
//
// stack declare 构建和释放 : 
// 构建 struct chars var = {}; 释放 free(var.str);
// heap  declare 构建和释放 : 
// 构建 struct chars * cs = calloc(1, sizeof(struct chars)); 释放 chars_delete(cs);
struct chars {
    char * str;     // char * 字符串
    size_t len;     // 长度
    size_t cap;     // capacity 容量
};

//
// chars_delete - struct chars * 释放函数
// cs       : 待释放的串对象
// return   : void
//
inline void chars_delete(struct chars * cs) {
    free(cs->str);
    free(cs);
}

//
// chars_expand - low level 字符串扩容 api
// cs       : 可变字符串
// len      : 扩容的长度
// return   : cstr::str + cstr::len 位置的串
//
char * chars_expand(struct chars * cs, size_t len);

inline void chars_appendc(struct chars * cs, int c) {
    assert(cs != NULL && c >= CHAR_MIN && c <= CHAR_MAX);
    chars_expand(cs, 1); cs->str[cs->len++] = c;
}

inline void chars_appendn(struct chars * cs, const char * str, size_t len) {
    assert(cs != NULL && str != NULL && len > 0);
    memcpy(chars_expand(cs, len), str, len);
    cs->len += len;
}

extern void chars_appends(struct chars * cs, const char * str);

//
// chars_create - struct chars * 创建函数, 根据 C 串创建 struct chars * 字符串
// str      : 待创建的字符串
// len      : 创建串的长度
// return   : 返回创建的字符串
//
inline struct chars * chars_create(const char * str, size_t len) {
    struct chars * cs = calloc(1, sizeof(struct chars));
    chars_appendn(cs, str, len);
    return cs;
}

inline struct chars * chars_creates(const char * str) {
    struct chars * cs = calloc(1, sizeof(struct chars));
    chars_appends(cs, str);
    return cs;
}

//
// chars_get - 通过 struct chars * 串得到一个 C 串以'\0'结尾. 
//            如果你很自信完全可以 cs->str 获取
// cs       : struct chars * 串
// return   : 返回构建 C 串, 内存地址 cs->str
//
inline char * chars_get(struct chars * cs) {
    chars_expand(cs, 1)[0] = 0;
    return cs->str;
}

//
// chars_dup - 得到 C 堆上的串, 需要自行 free
// cs       : struct chars * 串
// return   : 返回创建好的 C 串
//
extern char * chars_dup(struct chars * cs);

//
// chars_pop - 字符串头部分弹出 len 长度字符
// cs       : 可变字符串
// len      : 弹出的长度
// return   : void
//
extern void chars_pop(struct chars * cs, size_t len);

//
// chars_sprintf - 参照 sprintf 方式填充内容
// cs       : struct chars * 串
// fmt      : 待格式化的串
// ...      : 可变参数列表
// return   : 返回创建的 C 字符串内容
//
extern char * chars_sprintf(struct chars * cs, const char * fmt, ...) __attribute__((format(printf, 2, 3))) ;
