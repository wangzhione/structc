#pragma once

#include "struct.h"

// 轻量 char * 串
//
// declare 构建 : struct chars var = {};
// free         : free(var.str);
// 
struct chars {
    size_t len;     // 长度
    size_t cap;     // capacity 容量
    char * str;     // char * 字符串
};

//
// chars_expand 字符池子扩容 api
// cs       : 可变字符串
// len      : 扩容的长度
// return   : chars::str + chars::len 位置的串
//
extern char * chars_expand(struct chars * cs, size_t len);

//
// chars_get - 通过 struct chars * 串结构得到一个 以 '\0' 结尾 C 串. 
//            如果你很自信完全可以 chars::str 获取
// cs       : struct chars * 串
// return   : 返回构建 C 串, 内存地址 chars::str
//
inline char * chars_get(struct chars * cs) {
    chars_expand(cs, 1)[0] = 0;
    return cs->str;
}

inline void chars_appendc(struct chars * cs, int c) {
    assert(cs != nullptr && c >= CHAR_MIN && c <= CHAR_MAX);
    chars_expand(cs, 1)[0] = c;
    cs->len++;
}

inline void chars_appendn(struct chars * cs, const char * str, size_t len) {
    assert(cs != nullptr && str != nullptr);
    memcpy(chars_expand(cs, len), str, len);
    cs->len += len;
}

inline void chars_appends(struct chars * cs, const char * str) {
    chars_appendn(cs, str, strlen(str));
}

//
// chars_dup - 得到 C 堆上的串, 需要自行 free
// cs       : struct chars * 串
// return   : 返回创建好的 C 串
//
extern char * chars_dup(struct chars * cs);

//
// chars_pop - 字符串头弹出 len 长度字符
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
