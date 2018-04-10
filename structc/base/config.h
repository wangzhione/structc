#ifndef _H_CONFIG
#define _H_CONFIG

#include <stdbool.h>

//
// config 映射配置 config/config.json
//
struct config {
    char * description;
    char * image;
};

//
// config_instance - 获取配置
// return   : 返回详细配置内容
//
extern struct config * config_instance(void);

//
// config_init - 初始化读取配置内容
// return   : true 表示解析成功
//
extern bool config_init(void);

#endif//_H_CONFIG
