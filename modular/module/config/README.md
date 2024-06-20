# structc module config

所有配置相关存在在这里

## modular/config/config.json.conf 配置

config.h / config.c 与 modular/config/config.json.conf 一一映射. 添加修改见 **config_parse**

```C
// conf_parse - 解析内容, 并返回解析结果
static bool config_parse(json_t json, struct conf * conf) {
    CONFIG_PARSE_JSON_STR(json, conf, description);
    CONFIG_PARSE_JSON_STR(json, conf, image);

    // ... .. .

    return true;
}
```

## 更多

... .. .
