# structc 项目构建说明

    作者 : csharpwz@outlook.com

***

-> [凄美的](http://music.163.com/#/m/song?id=436346833&userid=16529894)

***

## 0. 存在合理性

    a). 条理清晰的用 C 去提供服务
    b). 减少开发成本, 用 winds 测试, linux 发布
    c). 平台采用x64下 winds DEBUG, linux release

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

        $(ProjectDir)base			 -- 基于上面的基础核心模块
        $(ProjectDir)struct			 -- 基础数据结构提供
        $(ProjectDir)system			 -- 操作系统一些通用操作
        $(ProjectDir)system/uv		 -- 网络IO操作, 内嵌libuv
		$(ProjectDir)system/pthread  -- POSIX pthread 线程模型

    d). 添加预编译处理器
        项目右击 -> [属性] -> [C/C++]-> [预处理器] -> [预处理器定义]

        _DEBUG
		PTW32_STATIC_LIB
		JEMALLOC_STATIC
		JEMALLOC_EXPORT=
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        _WINSOCK_DEPRECATED_NO_WARNINGS

        [Release]
        - _DEBUG

    e). 设置编译额外选项
        项目右击 -> [属性] -> [C/C++] -> [常规] -> [调试信息格式] -> [程序数据库 (/Zi)]
        项目右击 -> [属性] -> [C/C++] -> [高级] -> [编译为] -> [编译为C代码/TC]
        项目右击 -> [属性] -> [C/C++] -> [代码生成] -> [运行库] -> [多线程/MT]
        项目右击 -> [属性] -> [链接器] -> [常规] -> [启用增量链接] -> [否 (/INCREMENTAL:NO)]
        项目右击 -> [属性] -> [链接器] -> [系统] -> [子系统] -> [控制台]
        项目右击 -> [属性] -> [链接器] -> [命令行] -> /LTCG 

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

    不妨采用Best New Ubuntu x64 GCC环境搭建

    a) 依赖安装

```Bash
# 开发环境安装
sudo apt install gcc gdb autogen autoconf

# jemalloc 安装
cd
wget https://github.com/jemalloc/jemalloc/releases/download/5.0.1/jemalloc-5.0.1.tar.bz2
tar -jxvf jemalloc-5.0.1.tar.bz2
cd jemalloc-5.0.1

sh autogen.sh

make -j4

sudo make install
sudo ldconfig
cd ../
rm -rf jemalloc-5.0.1 jemalloc-5.0.1.tar.bz2

# libuv 安装
cd
wget https://github.com/libuv/libuv/archive/v1.18.0.tar.gz
tar -zxvf v1.18.0.tar.gz
cd libuv-1.18.0

sh autogen.sh
./configure

make -j4

sudo make install
sudo ldconfig
cd ../
rm -rf libuv-1.18.0 v1.18.0.tar.gz
```

    b) 编译设置
		
       -static
       -lm 
	   -luv
       -lpthread

    c) 踩坑
        
        gcc -I 会诱发 stdext.o 没有嵌入 jemalloc 相关代码, 使用下面方式

```bash
gcc -g -O2 -Wall -Wno-unused-result -c stdext.c -DJEMALLOC_NO_DEMANGLE -l:libjemalloc.a
```

        解决编译无法内嵌 jemalloc 问题!
