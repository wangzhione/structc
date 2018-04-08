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
// config_parse - 初始化配置信息
// path     : 配置路径
// outf     : 返回的配置解析内容
// return   : true 表示读取成功
//
extern bool config_parse(const char * path, struct config * outf);

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
