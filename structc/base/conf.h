#ifndef _CONF_H
#define _CONF_H

#include "utf8.h"
#include "json.h"

//
// config 映射配置
//
struct conf {
    char * description;
    char * image;
};

//
// conf_instance - 获取配置
// return   : 返回详细配置内容
//
extern struct conf * conf_instance(void);

//
// conf_init - 初始化读取配置内容
// path     : 配置初始化路径
// return   : true 表示解析成功
//
bool conf_init(const char * path);

#endif//_CONF_H
