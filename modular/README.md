﻿# structc 项目构建说明

    作者 : 喜欢兰花山丘
    邮箱 : csharpwz@outlook.com

***

--> [北方的故事](http://music.163.com/#/song?id=37782112)

***

## 0. 存在合理性

    a). 条理清晰的让 C 去提供服务, 减少开发成本

## 1. linux 部署

    推荐采用 Best New Ubuntu x64 GCC 环境搭建

    a) 编译设置

        -lm -lpthread

    b). 单元测试规则

        {接口}.h -> test :: {接口}_test.c ->

            {接口}_test(void) {
                ...
                ..
                .
            }

        测试文件. 方便统一生成特定内容.

## 2. 设计交流

C 中 很多结构, 用数组和链表以及 char * 也许都能简单的构造出来. 非常灵活. 
在其中引入通用复杂结构有时候是业务负担, 他太强调针对性了. 

## 3. 未来 🙋 天人合一

![缺一不可](./../extern/Flag_of_Ukraine_(pantone_colors).svg.png)
