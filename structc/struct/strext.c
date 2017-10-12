#include "strext.h"
#include "elag.h"

// str_hash - Brian Kernighan与 Dennis Ritchie 简便快捷的 hash算法
// str		: 字符串内容
// return	: 返回计算后的hash值
unsigned 
str_hash(const char * str) {

	return 0u;
}

// str_icmp - 字符串不区分大小写比较函数
// ls		: 左串
// rs		: 右串
// return	: ls > rs 返回 > 0; ... < 0; ... =0
int 
str_icmp(const char * ls, const char * rs) {
	return 0;
}

// str_dup - 字符串拷贝malloc函数, 需要自己free
// str		: 待拷贝的串
// return	: 返回拷贝后的串
char * 
str_dup(const char * str) {
	return NULL;
}

// str_freadend - 简单的文件读取类,会读取完毕这个文件内容返回, 需要自己free
// path		: 文件路径
// return	: 创建好的字符串内容, 返回NULL表示读取失败
char * 
str_freadend(const char * path) {
	return NULL;
}

// str_fwrites - 将c串str覆盖写入到path路径的文件中
// path		: 文件路径
// str		: c的串内容
// return	: 0 is success, < 0 is error
int 
str_fwrites(const char * path, const char * str) {
	return 0;
}

// tstr_fappends - 将c串str写入到path路径的文件中末尾
// path		: 文件路径
// str		: c的串内容
// return	: 0 is success, < 0 is error
int 
tstr_fappends(const char * pat, const char * str) {
	return 0;
}