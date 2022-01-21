﻿# STRUCT C 基础库

    雕琢 C 的华山剑法, 为那会的一时性起 ~>--

## Update

| 当前时间       | 下步计划                                          | 进展情况 |
| :------------: | :-----------------------------------------------: | :------: |
| 2021年02月24日 | 等最新 Visual Studio 正式版发布, 筹备浴火重生     | 等待     |
| 2021年12月30日 | 回归简单友好 Linux 开发                           | 开始     |
| 2022年01月19日 | C 菜鸟之旅 版本发布                               | 新生     |
|                |                                                   |          |

## 浪花不会一点点

起始点:

| 优先级 | 运算符       | 描述                            |  结合性  |
| :----: | :----------- | :------------------------------ | :------: |
|   1    | ++ --        | 后缀自增与自减                  | 从左到右 |
|        | ()           | 函数调用                        |          |
|        | []           | 数组下标                        |          |
|        | .            | 结构体与联合体成员访问          |          |
|        | ->           | 结构体与联合体成员通过指针访问  |          |
|        | (type){list} | 复合字面量                      |          |
|   2    | ++ --        | 前缀自增与自减                  | 从右到左 |
|        | + -          | 一元加与减                      |          |
|        | ! ~          | 逻辑非与逐位非                  |          |
|        | *            | 间接(解引用)                    |          |
|        | &            | 取址                            |          |
|        | sizeof       | 取大小                          |          |
|        | _Alignof     | 对齐要求                        |          |
|   3    | * / %        | 乘法及除法及余数                | 从左到右 |
|   4    | + -          | 加法及减法                      | 从左到右 |
|   5    | << >>        | 逐位左移及右移                  | 从左到右 |
|   6    | < <=         | 分别为 < 与 ≤ 的关系运算符      | 从左到右 |
|        | > >=         | 分别为 > 与 ≥ 的关系运算符      | 从左到右 |
|   7    | == !=        | 分别为 = 与 ≠ 关系              | 从左到右 |
|   8    | &            | 逐位与                          | 从左到右 |
|   9    | ^            | 逐位异或(排除或)                | 从左到右 |
|   10   | \|           | 逐位或(包含或)                  | 从左到右 |
|   11   | &&           | 逻辑与                          | 从左到右 |
|   12   | \|\|         | 逻辑或                          | 从左到右 |
|   13   | ?:           | 三元条件                        | 从右到左 |
|   14   | =            | 简单赋值                        | 从右到左 |
|        | += -=        | 以和及差赋值                    |          |
|        | *= /= %=     | 以积及商及余数赋值              |          |
|        | <<= >>=      | 以逐位左移及右移赋值            |          |
|        | &= ^= \|=    | 以逐位与及异或及或赋值          |          |
|   15   | ,            | 逗号                            | 从左到右 |

核心点:

    struct 数据结构练习包
    system 系统封装排练包
    module 业务模块

总设计:

    struct, json, socket, thread ...

    来协助搭建 C 项目, 提供基础数据结构和额外组件的支持 ...

    ...

**代码风格, 希望"最"贴近原生 api**

```C
#pragma once

#include <pthread.h>
#include <semaphore.h>

#include "struct.h"
#include "spinlock.h"

//
// pthread_async - 启动无需等待的线程
// frun     : node_f or ... 运行主体
// arg      : 运行参数
// return   : 0 is success, -1 is error
// 
extern int pthread_async(void * frun, void * arg);

//
// thread_async - 强制启动无需等待线程
// frun     : node_f or ... 运行主体
// return   : void 
//
inline void thread_async(void * frun) {
    IF(pthread_async(frun, NULL));
}

//
// pthread_run - 启动线程
// p        : 指向线程 id 的指针
// frun     : node_f or ... 运行主体
// arg      : 运行参数
// return   : 0 is success, -1 is error
//
inline int pthread_run(pthread_t * p, void * frun, void * arg) {
    return pthread_create(p, NULL, frun, arg);
}

//
// pthread_end - 等待线程运行结束
// tid      : 线程 id
// return   : void
//
inline void pthread_end(pthread_t id) {
    pthread_join(id, NULL);
}
```

***

### 也许要扯些

[structc 环境搭建](./modular/README.md)

    - Linux 回车
        - Debug   : make
        - Clean   : make clean
        - Release : make D=-DNDEBUG

    - 工程目录述
        - main 目录
            - main::exec.c 业务起点
            - main::init.c 模块初始化
            - main::test.c test 单元测试
        - test 目录
            - *_test.c -> void *_test(void) { ... }

***

### IDE 弱议

    Linux 抛砖引玉带大家配哈 VS Code Best Version

    0. 安装 C/C++ Microsoft 插件

    1. F1 -> Edit Configurations -> c_cpp_properties.json

```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceRoot}/modular/main",
                "${workspaceRoot}/modular/module",
                "${workspaceRoot}/modular/struct",
                "${workspaceRoot}/modular/system"
            ],
            "defines": [
                "_DEBUG",
                "__GNUC__",
                "__linux__",
                "_GNU_SOURCE"
            ],
            "compilerPath": "/usr/bin/gcc"
        }
    ],
    "version": 4
}
```

    2. F5 -> C++(GDB/LLDB) -> launch.json
       按照规律改 program 项目生成 和添加 preLaunchTask 前置任务行

```json
            "program": "${workspaceFolder}/bin/main.out",
            ... .. .
            "preLaunchTask": "Debug",
```

    3. F1 -> [>Tasks: Configure Task] -> [Others] -> tasks.json
       构建下面任务

```json
{
    "tasks": [
        {
            "type"    : "shell",
            "label"   : "Debug",
            "command" : "make"
        },
        {
            "type"    : "shell",
            "label"   : "Release",
            "command" : "make D=-DNDEBUG"
        },
    ],
    "version": "2.0.0"
}
```

    4. [settings.json] 设置 *.h 是 C 文件格式

```json
{
    "files.associations": {
        "*.h": "c",
    }
}
```

    核心思路是配置 VSCode C Studio 联动 Makefile Debug 调试.
    此刻有了这些, 有兴趣的就可以 F5 F10 F11 随心所欲开始冒险之旅.

    总的而言是要比 vi + gcc + gdb 好那么一丢丢 ~ ❤

***

## ~ * ~ 为要寻一个写手

    为要寻一个明星
    - 徐志摩

    我骑着一匹拐腿的瞎马，
    向着黑夜里加鞭；——
    向着黑夜里加鞭，
    我跨着一匹拐腿的瞎马！

    我冲入这黑绵绵的昏夜，
    为要寻一颗明星；——
    为要寻一颗明星，
    我冲入这黑茫茫的荒野。

    累坏了，累坏了我胯下的牲口，
    那明星还不出现；——
    那明星还不出现，
    累坏了，累坏了马鞍上的身手。

    这回天上透出了水晶似的光明，
    荒野里倒着一只牲口，
    黑夜里躺着一具尸首。——
    这回天上透出了水晶似的光明！

***

```C
...................o888888o.o88888888o.
.................8888888888888888888888888o.
...............888::::888888888888888888888888o.
.............88::::::::88888888888888888888888888o.
...........o8:::::::::888::顶88888888888888888888888.
..........8888:8::::::8:::::::::顶88888888888888888888.
.........8888::::8888::::::::::::::::顶88888888888 '88
........8888:::888888888::::::::::::::::顶888888888 8
.......88.88888888888888888:::::::::::::顶88888888888
......88..888888888888888888:::::::::::顶88888888888888
......88.888888888888888888888::::::::顶88888888888888888
......8..8888888888888888888888::::::顶88888888888888888888
........8888888888888888::88888:::::顶8888888888888888888888
......88888888888888888::::8888::::顶8888888888888888 '8888
.....88888888888888888::::8888::::顶:8::***::888888888 888
....88888888888888888::::888:::::顶::::*EYE*::888888888 88
..888888888888888888::::88::::::顶:::::::::::::888888888 88
.8888888888888888888::::8::::::顶:::::::::::::顶888888888 88
888...8888888888::88::::8:::::顶:::8oo8::::::88888888888888 8
88...88888888888:::8:::::::::顶:::::::::::::88888888888888888
.8..8888888888888:::::::::::顶::"8888888":::顶88888888888888888
..8888888888::888::::::::::顶:::::"8a8"::::顶88888888888888888888
.888888888:::::88:::::::::顶88::::::::::::顶88888888888888888888888
8888888888:::::8:::::::::顶88888::::::::顶888888888888888888888888888
888888888:::::8:::::::::顶8888888ooooo顶8888888888888888888888888888888
888888.顶::::::::::::::顶8888888888::::::顶8888888888888888 '888888888
8888..顶:::::::::::::::顶88888888888::::::顶8888888888888888 88888888
.888..顶::::::::::::::顶8888888888888:::::::顶888888888888888 888888
..888.顶:::::::::::::顶88888888888888888::::::顶8888888888888 88888
...88.顶:::::::::::::8888:88888888888888888:::::顶顶888888888 8888
...88.顶:::::::::::8888顶::88888::888888888888:::::顶顶88888 888
...8...顶::::::::::8888顶:::8888:::::88888888888::::::顶顶88 888
.......8顶:::::::8888顶:::::888:::::::88:::8888888:::::::顶顶 88
......88顶:::::::8888顶::::::88:::::::::8:::::888888::::::::顶顶 88
.....8888顶:::::888顶:::::::::88::::::::::顶::::8888::::::::::::顶 8
....88888:顶::::888顶::::::::::8:::::::::::顶::::888::::::::::::::顶
...88.888:顶:::888:顶::::::::::8::::::::::::顶::888::::::::::::::::顶
...8.88888顶:::88::顶::::::::::::::::::::::::顶:88::::::::::::::::::顶
.....88888顶:::88::顶:::::::::::::::::::::::::顶88:::::::::::::::::::顶
.....88888顶:::88::顶:::::::::::::::::::::::::顶:8::::::::::::::::::::顶
....888888顶::::8::顶::::::::::::::8888::::::::顶8::::::::::::8888:::::顶
....88888..顶:::8::顶顶:::::::::::888888:::::::顶::::::::::::888888::::顶
....8888...顶顶:::::顶顶::::::::::888888:::::::顶顶::::::::::888888::::顶
....8888....顶:::::::顶顶::::::::::8888:::::::顶顶顶::::::::::8888::::顶'
.....888....顶顶::::::顶顶顶:::::::::::::::::顶顶::顶::::::::::::::::顶'
......88.....顶::::::::顶顶顶顶:::::::::::顶顶顶:::顶顶:::::::::::::顶'
.......88....顶顶:::::::::顶顶顶顶顶顶顶顶顶顶:::::::顶顶顶顶顶顶顶顶'
........88....顶顶::::::::::::顶顶顶顶顶顶顶::::::::::::顶顶顶顶顶o'
.........88...8顶顶::::::::::::::::::::::::::::::::::::::::顶
..........8...88顶顶::::::::::::::::::::::顶:::顶::::::::::顶
..............888顶顶::::::::::::::::::顶顶::::::顶顶::::::顶
.............88888顶顶:::::::::::::::顶顶顶:::::::顶顶:::::顶
.............888888顶顶:::::::::::::顶顶顶:::::::::顶顶:::顶
............88888888顶顶:::::::::::顶顶顶:::::::::::顶:::顶
...........88.8888888.顶::::::::顶顶顶::::::::::::::顶::顶
...........8..888888..顶::::::顶顶:::::::::::::::::顶::顶
..............888888..顶:::::顶:::::::::::::::::::顶::顶顶
.............888888...顶::::顶::::::::::::::::::::::::顶顶
.............888888...顶::::顶::::::::::::::::::::::::顶:顶
.............88888...顶::::::::::::::8::::::::::::::::顶::顶
............88888...顶:::::::::::::::88::::::::::::::::顶::顶
...........88888...顶::::::::::::::::8::::::::::::::::::顶::顶
..........88888...顶:::::顶:::::::::::::::::::::顶::::::::顶::顶
..........8888...顶:::::顶:::::::::::::::::::::::顶顶::::::顶::顶
.........8888...顶:::::顶:::::::::::::::::::::::顶顶顶:::::::顶:顶
........888....顶:::::顶顶::::::::::::::::::::::顶顶顶::::::::顶:顶
......8888....顶::::::顶顶:::::::::::::::::::::顶顶顶::::::::: 顶:顶
.....888......顶:::::顶:::::::::::::::::::::::顶顶::::::::::::::顶:顶
..8888.......顶::::::::::::::::::::::::::::::顶::::::::::::::::::顶:顶
.............顶::::::::::::::::::::::::::::::::::::::::::::::::::顶:顶
............顶::::::::顶:::::::::::::::::::::::::::::::::::::::::顶:顶
............顶::::::::顶:::::::::::::::::顶::::::::::::::::::::::顶:顶
...........顶::::::::::顶::::::::::::::顶::::::::::::::::::::::::顶:顶
...........顶:::::::::::顶8::::::::8:顶:::::::::::::::::::::::::顶:顶p
...........顶::::::::::::888hole888顶:::::::::::::::::::::::::::顶:顶o
...........顶:::::::::::::88888888顶:::::::::::::::::::::::::::顶:顶p
...........顶::::::::::::::888888顶::::::::::::::::::::::::::::顶:顶
...........顶:::::::::::::::88888顶:::::::::::::::::::::::::::顶:顶
...........顶:::::::::::::::::88顶::::::::::::::::::::::::::::顶顶
...........顶:::::::::::::::::::顶:::::::::::::::::::::::::::顶顶
...........顶:::::::::::::::::::顶::::::::::::::::::::::::::顶顶
............顶:::::::::::::::::顶::::::::::::::::::::::::::顶顶
............顶:::::::::::::::::顶::::::::::::::::::::::::::顶
.............顶:::::::::::::::顶::::::::::::::::::::::::::顶
.............顶:::::::::::::::顶:::::::::::::::::::::::::顶
..............顶:::::::::::::顶:::::::::::::::::::::::::顶
..............顶:::::::::::::顶:::::::::::::::::::::::顶
...............顶:::::::::::顶:::::::::::::::::::::::顶
...............顶:::::::::::顶::::::::::::::::::::::顶
................顶:::::::::顶::::::::::::::::::::::顶
................顶:::::::::顶:::::::::::::::::::::顶
.................顶:::::::顶:::::::::::::::::::::顶
..................顶::::::顶::::::::::::::::::::顶
..................顶:::::顶::::::::::::::::::::顶
...................顶:::顶::::::::::::::::::::顶
...................顶::顶::::::::::::::::::::顶
....................顶:顶:::::::::::::::::::顶
....................顶顶:::::::::::::::::::顶
....................顶顶::::::::::::::::::顶
....................顶顶:::::::::::::::::顶
....................顶顶:::::顶::::::::顶顶
....................顶顶::::顶顶:::::::顶顶
....................顶顶::::::顶顶:::::顶顶
.....................顶顶::::::::::::::顶顶
.....................顶顶::::顶:::::::::顶顶
......................顶顶::顶顶顶:::::::顶顶
......................顶顶::顶顶顶:::::::顶顶
......................顶顶:::顶顶::::::::::顶
.......................顶顶::顶顶::::::::::顶
.......................顶顶::顶顶:::::::::::顶
.......................顶顶:::顶::::::::::::顶
.......................顶顶:::::::::::::::::顶
.......................顶顶:::::::::::::::::顶
.......................顶顶::::::::::::::::::顶
.......................顶顶::::::::::::::::::顶
.......................顶顶::::::::::::::::::顶
........................顶顶:::::::::::::::::顶
........................顶顶:::::::::::::::::顶
........................顶顶:::::::::::::::::顶
........................顶顶:::::::::::::::::顶
........................顶顶:::::::::::::::::顶
.........................顶顶:::::::::::::::顶
.........................顶顶:::::::::::::::顶
.........................顶顶:::::::::::::::顶
.........................顶顶::::::::::::::顶
..........................顶顶:::::::::::::顶
..........................顶顶:::::::::::::顶
..........................顶顶:::::::::::::顶
..........................顶顶::::::::::::顶
..........................顶顶::::::::::::顶
...........................顶顶:::::::::::顶
...........................顶顶::::::::::顶
...........................顶顶::::::::::顶
............................顶顶::::::::顶
............................顶顶::::::::顶
............................顶顶::::::::顶
.............................顶顶:::::::顶
.............................顶顶:::::::顶
..............................顶顶::::::顶
..............................顶顶::::::顶
..............................顶顶::::::顶
..............................顶顶::::::顶
..............................顶顶::::::顶
..............................顶顶::::::顶
.............................顶顶:::::::顶
............................顶:顶::::::::顶
............................顶:顶:::::::::顶
............................顶:顶::::::::::顶
............................顶:顶:::::::::::顶
............................顶:顶::::::::::::顶
............................顶:顶:::::::::::::顶
............................顶:顶::::::::::::::顶
............................顶顶:::::::::::::::::顶
..............................顶顶::::::::::::::::顶
................................顶顶顶:::::::::顶顶
..................................顶顶::顶::::顶顶
...................................顶顶顶顶顶顶顶
......................................顶顶顶顶顶
```

***

    :)

***

-:> [Wuayramanta](https://i.y.qq.com/v8/playsong.html?songid=5012912&songtype=0#webchat_redirect)

***
