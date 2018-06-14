#ifndef _H_CHECK
#define _H_CHECK

#include <stdbool.h>

//
// is_ip - 判断是否是 ip
// ips      : ip 串
// return   : true 是合法 ip
//
extern bool is_ip(const char * ips);

//
// is_email - 判断是否是邮箱
// mail     : email 串 
// return   : true is email
//
extern bool is_email(const char * mail);

#endif//_H_CHECK
