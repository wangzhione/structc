#ifndef __STRUCTC_BASE_CHECK_H
#define __STRUCTC_BASE_CHECK_H

#include <stdbool.h>

//
// is_ipv4 - 判断是否是 ipv4
// ips      : ip 串
// return   : true 是合法 ip
//
extern bool is_ipv4(const char * ips);

//
// is_email - 判断是否是邮箱
// mail     : email 串 
// return   : true is email
//
extern bool is_email(const char * mail);

#endif//__STRUCTC_BASE_CHECK_H
