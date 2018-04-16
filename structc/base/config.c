#include <json.h>
#include <chead.h>
#include <config.h>

//
// config_instance - 获取配置
// return   : 返回详细配置内容
//
inline struct config * 
config_instance(void) {
    //
    // 配置存储信息
    //
    static struct config conf;

    return &conf;
}

// CONFIG_PARSE_JSON_STR - json field -> config field
#define CONFIG_PARSE_JSON_STR(json, conf, field)                        \
json_t $##field = json_object(json, #field);                            \
if (NULL == $##field || $##field->type != JSON_STRING) {                \
    RETURN(false, "json_object err field = %s, %p", #field, $##field);  \
}                                                                       \
free(conf->field);                                                      \
conf->field = json_vals($##field);                                      \
os_cstr(conf->field)

// config_parse - 解析内容, 并返回解析结果
bool config_parse(json_t json, struct config * conf) {
    CONFIG_PARSE_JSON_STR(json, conf, description);
    CONFIG_PARSE_JSON_STR(json, conf, image);

    return true;
}

//
// config_init - 初始化读取配置内容
// path     : 配置初始化路径
// return   : true 表示解析成功
//
bool 
config_init(const char * path) {
    bool ret;
    json_t json = json_file(path);
    if (NULL == json) {
        RETURN(false, "json_file err path is %s", path);
    }

    // 解析 json 内容, 并返回详细配置内容
    ret = config_parse(json, config_instance());
    json_delete(json);
    return ret;
}
