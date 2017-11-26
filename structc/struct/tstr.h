#ifndef _H_TSTR
#define _H_TSTR

#include <strext.h>

#ifndef _STRUCT_TSTR

struct tstr {
    unsigned len;   // 长度
    unsigned cap;   // 容量
    char * str;     // 字符池
};

typedef struct tstr * tstr_t;

//
// TSTR_CREATE - 堆上创建tstr_t结构
// TSTR_DELETE - 释放堆上tstr_t结构
// var  : 变量名
//
#define TSTR_CREATE(var) \
struct tstr var[1] = { { 0, 0, NULL } }
#define TSTR_DELETE(var) \
free((var)->str)

#define _STRUCT_TSTR
#endif//_STRUCT_TSTR

//
// tstr_t 创建函数, 会根据c的tstr串创建一个tstr_t结构的字符串
// str		: 待创建的字符串
// len		: 创建串的长度
// return	: 返回创建好的字符串,内存不足会打印日志退出程序
//
extern tstr_t tstr_create(const char * str, unsigned len);
extern tstr_t tstr_creates(const char * str);

//
// tstr_delete - tstr_t 释放函数
// tstr		: 待释放的串结构
// return   : void
//
extern void tstr_delete(tstr_t tstr);

//
// 向tstr_t串结构中添加字符等, 内存分配失败内部会自己处理
// c		: 单个添加的char
// str		: 添加的c串
// sz		: 添加串的长度
//
extern void tstr_appendc(tstr_t tstr, int c);
extern void tstr_appends(tstr_t tstr, const char * str);
extern void tstr_appendn(tstr_t tstr, const char * str, unsigned sz);

//
// tstr_cstr - 通过cstr_t串得到一个c的串以'\0'结尾
// tstr		: tstr_t 串
// return	: 返回构建好的c的串, 内存地址tstr->str
//
extern char * tstr_cstr(tstr_t tstr);

//
// tstr_expand - 为当前字符串扩容, 属于低级api
// tstr		: 可变字符串
// len		: 扩容的长度
// return	: tstr->str + tstr->len 位置的串
//
char * tstr_expand(tstr_t tstr, unsigned len);

//
// tstr_popup - 从字符串头弹出len长度字符
// tstr		: 可变字符串
// len		: 弹出的长度
// return	: void
//
extern void tstr_popup(tstr_t tstr, unsigned len);

//
// tstr_dupstr - 得到一个精简的c的串, 需要自己事后free
// tstr		: tstr_t 串
// return	: 返回创建好的c串
//
extern char * tstr_dupstr(tstr_t tstr);

//
// tstr_printf - 参照 sprintf 填充方式写入内容
// tstr     : tstr_t 串
// format   : 待格式化的串
// ...      : 等待进入的变量
// return   : 返回创建好的C字符串内容
//
extern char * tstr_printf(tstr_t tstr, const char * format, ...);

#endif//_H_TSTR
