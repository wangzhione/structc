#ifndef _TSTR_H
#define _TSTR_H

#include "strext.h"

#ifndef TSTR_CREATE

struct tstr {
    char * str;   // 字符池
    size_t cap;   // 容量
    size_t len;   // 长度
};

// TSTR_INT 字符串构建的初始大小
#define TSTR_INT  (1<<8)

typedef struct tstr * tstr_t;

//
// TSTR_CREATE - 栈上创建 tstr_t 结构
// TSTR_DELETE - 释放栈上 tstr_t 结构
// var  : 变量名
//
#define TSTR_CREATE(var)                \
struct tstr var[1] = { {                \
    .str = malloc(TSTR_INT),            \
    .cap = TSTR_INT,                    \
} }

#define TSTR_DELETE(var)                \
free((var)->str)

#endif//TSTR_CREATE

//
// tstr_delete - tstr_t 释放函数
// tsr      : 待释放的串结构
// return   : void
//
extern void tstr_delete(tstr_t tsr);

//
// tstr_expand - 字符串扩容, low level api
// tsr      : 可变字符串
// len      : 扩容的长度
// return   : tsr->str + tsr->len 位置的串
//
char * tstr_expand(tstr_t tsr, size_t len);

//
// tstr_t 创建函数, 根据 C 串创建 tstr_t 字符串
// str      : 待创建的字符串
// len      : 创建串的长度
// return   : 返回创建的字符串
//
extern tstr_t tstr_create(const char * str, size_t len);
extern tstr_t tstr_creates(const char * str);

//
// tstr_t 串结构中添加字符等
// tsr      : tstr_t 串
// c        : 添加 char
// str      : 添加 char *
// sz       : 添加串的长度
// return   : void
//
extern void tstr_appendc(tstr_t tsr, int c);
extern void tstr_appends(tstr_t tsr, const char * str);
extern void tstr_appendn(tstr_t tsr, const char * str, size_t sz);

//
// tstr_cstr - 通过 str_t 串得到一个 C 串以'\0'结尾
// tsr      : tstr_t 串
// return   : 返回构建 C 串, 内存地址 tsr->str
//
extern char * tstr_cstr(tstr_t tsr);

//
// tstr_dupstr - 得到 C 的串, 需要自行 free
// tsr      : tstr_t 串
// return   : 返回创建好的 C 串
//
extern char * tstr_dupstr(tstr_t tsr);

//
// tstr_popup - 字符串头弹出 len 长度字符
// tsr      : 可变字符串
// len      : 弹出的长度
// return   : void
//
extern void tstr_popup(tstr_t tsr, size_t len);

//
// tstr_printf - 参照 sprintf 方式填充内容
// tsr      : tstr_t 串
// fmt      : 待格式化的串
// ...      : 等待进入的变量
// return   : 返回创建的 C 字符串内容
//
extern char * tstr_printf(tstr_t tsr, const char * fmt, ...);

#endif//_TSTR_H
