﻿#pragma once

#include "utf8.h"
#include "json.h"

//
// config 映射配置
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
// path     : 配置初始化路径
// return   : true 表示解析成功
//
bool config_init(const char * path);
