# structc 项目构建说明

    作者 : csharpwz@outlook.com

***

-> [凄美的](http://music.163.com/#/m/song?id=436346833&userid=16529894 )

***

## 0. 存在合理性

    a). 条理清晰的用C去提供服务
    b). 减少开发成本, 用winds测试, linux发布
    c). 平台采用x64下 winds DEBUG, linux release

## 1. winds 部署

    a). 使用 Best New CL
    b). 只保留 X64 DEBUG 环境
        项目右击 -> [属性] -> [配置管理器] -> 移除所有其它环境
    c). 添加包含目录
        项目右击 -> [属性] -> [VC++ 目录] -> [包含目录]

        $(ProjectDir)struct     -- 基础数据结构提供
        $(ProjectDir)system     -- 操作系统一些通用操作
        $(ProjectDir)base       -- 基于上面的基础核心模块

    d). 设置编译额外选项
        项目右击 -> [属性] -> [C/C++] -> [高级] -> [编译为] -> [编译为C代码/TC]
        项目右击 -> [属性] -> [C/C++] -> [代码生成] -> [运行库] -> [多线程/MT]
        项目右击 -> [属性] -> [链接器] -> [系统] -> [子系统] -> [控制台]

    e). 添加预编译处理器
        项目右击 -> [属性] -> [C/C++]-> [预处理器] -> [预处理器定义]

        _DEBUG
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        _WINSOCK_DEPRECATED_NO_WARNINGS

    f). 单元测试规则

        {接口}.h -> test :: {接口}_test.c ->

            {接口}_test(void) {
                ...
                ..
                .
            }

        测试文件. 方便统一生成特定内容.

    g). 后期生成事件
        项目右击 -> [属性] -> [生成事件] -> [后期生成事件] -> [命令行]

### 2. linux 部署
