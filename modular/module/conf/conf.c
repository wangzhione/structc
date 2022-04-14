﻿#include "conf.h"

//
// conf_instance - 获取配置
// return   : 返回详细配置内容
//
inline struct conf * conf_instance(void) {
    //
    // 配置存储信息
    //
    static struct conf conf;

    return &conf;
}

// CONFIG_PARSE_JSON_STR - json field -> conf field
#define CONFIG_PARSE_JSON_STR(json, conf, field)              \
json_t $##field = json_object(json, #field);                  \
if (!$##field || $##field->type != JSON_STRING) {             \
    RETURN(false, "json_object error "#field" %p", $##field); \
}                                                             \
free(conf->field);                                            \
conf->field = json_detach_str($##field);

// conf_parse - 解析内容, 并返回解析结果
static bool conf_parse(json_t json, struct conf * conf) {
    CONFIG_PARSE_JSON_STR(json, conf, description);
    CONFIG_PARSE_JSON_STR(json, conf, image);

    // ... .. .

    return true;
}

//
// conf_init - 初始化读取配置内容
// path     : 配置初始化路径
// return   : true 表示解析成功
//
bool conf_init(const char * path) {
    json_t json = json_file(path);
    if (!json) {
        RETURN(false, "json_file error path is %s", path);
    }

    // 解析 json 内容, 并返回详细配置内容
    bool ret = conf_parse(json, conf_instance());
    json_delete(json);
    return ret;
}
