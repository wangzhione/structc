# structc 项目构建说明

    作者 : 喜欢兰花山丘
    邮箱 : csharpwz@outlook.com

***

--> [北方的故事](http://music.163.com/#/song?id=37782112)

***

## 0. 存在合理性

    a). 条理清晰的让 C 去提供服务, 减少开发成本
    b). 平台采用 x64 下 winds DEBUG, linux release

## 1. winds 部署

    a). 使用 Best New CL | 只保留 x64 Debug and Release 环境
        项目右击 -> [属性] -> [配置管理器] -> 移除所有其它环境

    b). 添加包含库
        项目右击 -> [属性] -> [VC++ 目录] -> [库目录]

        项目右击 -> [属性] -> [链接器] -> [输入]

        libuv.lib
        psapi.lib
        user32.lib
        shell32.lib
        ws2_32.lib
        userenv.lib
        iphlpapi.lib
        advapi32.lib
        pthread_lib.lib
        jemalloc-vc141-Release-static.lib

        
    c). 添加包含目录
        项目右击 -> [属性] -> [VC++ 目录] -> [包含目录]

        $(ProjectDir)                   -- 基础核心模块
        $(ProjectDir)struct             -- 基础数据结构提供
        $(ProjectDir)system             -- 操作系统一些通用操作
        $(SolutionDir)extern            -- 引入的外部库文件路径

        项目右击 -> [属性] -> [VC++ 目录] -> [库目录]

        $(SolutionDir)extern            -- 引入的外部库文件路径

    d). 添加预编译处理器
        项目右击 -> [属性] -> [C/C++]-> [预处理器] -> [预处理器定义]

        _DEBUG

        _LARGEFILE_SOURCE
        _FILE_OFFSET_BITS=64
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        _WINSOCK_DEPRECATED_NO_WARNINGS    

        PTW32_STATIC_LIB

        JEMALLOC_STATIC
        JEMALLOC_EXPORT=

        [Release]
        - _DEBUG

    e). 设置编译额外选项
        项目右击 -> [属性] -> [C/C++] -> [常规] -> [调试信息格式] -> [程序数据库 (/Zi)]
        项目右击 -> [属性] -> [C/C++] -> [代码生成] -> [运行库] -> [多线程/MT]
        项目右击 -> [属性] -> [C/C++] -> [高级] -> [编译为] -> [编译为C代码/TC]
        项目右击 -> [属性] -> [C/C++] -> [高级] -> [禁用特定警告] -> [4098]
        项目右击 -> [属性] -> [链接器] -> [常规] -> [启用增量链接] -> [否 (/INCREMENTAL:NO)]
        项目右击 -> [属性] -> [链接器] -> [系统] -> [子系统] -> [控制台]
        项目右击 -> [属性] -> [链接器] -> [命令行] -> /LTCG /IGNORE:4099
        
        项目右击 -> [生成事件] -> [后期生成事件] -> [命令行]

        echo begin init directory $(TargetDir)conf
        if not exist  $(TargetDir)conf mkdir $(TargetDir)conf
        xcopy /D /S /E /Y $(ProjectDir)conf $(TargetDir)conf
        if not exist  $(TargetDir)test mkdir $(TargetDir)test
        if not exist  $(TargetDir)test\conf mkdir $(TargetDir)test\conf
        xcopy /D /S /E /Y $(ProjectDir)test\conf $(TargetDir)test\conf
        echo e n d init directory $(TargetDir)conf

        [Release]
        - [控制台]
        | [否 (/INCREMENTAL:NO)]
        | [程序数据库 (/Zi)]

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

## 2. linux 部署

    不妨采用 Best New Ubuntu x64 GCC 环境搭建

    a) 依赖安装

```Bash
# 开发环境安装
sudo apt install gcc gdb autogen autoconf libtool

# jemalloc 安装
cd
wget https://github.com/jemalloc/jemalloc/releases/download/5.1.0/jemalloc-5.1.0.tar.bz2
tar -jxvf jemalloc-5.1.0.tar.bz2
cd jemalloc-5.1.0

sh autogen.sh
make -j4
sudo make install
sudo ldconfig

cd
rm -rf jemalloc-5.1.0 jemalloc-5.1.0.tar.bz2

# libuv 安装
cd
wget https://github.com/libuv/libuv/archive/v1.24.1.zip
unzip v1.24.1.zip
cd libuv-1.24.1

sh autogen.sh
./configure
make -j4
sudo make install
sudo ldconfig

cd
#
# 注意 uv 头文件, 全部导入到系统 include 目录下面
#
rm -rf libuv-1.24.1 v1.24.1.zip
```

    b) 编译设置

        -lm
        -lpthread

        -luv
