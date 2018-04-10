# C STRUCT 基础库

    目的 : 构建一个稍微通用点的 c 结构体基础库
    方式 : 基于注册
    适用 : 用 c 从零开始搭建项目的人群, 或者激励自己真的想看看基础的同学

## 好像才刚开始 ~

-- [北方的故事](http://music.163.com/#/m/song?id=37782112&userid=16529894)

    核心围绕: 
        mq.h        队列
        dict.h      字典
        tstr.h      字符串
        list.h      单链表
        rtree.h     红黑树
        array.h     动态数组

    总的目标:
        json 串, 原子锁, ...
    
    来协助我们搭建多线程 C 项目, 提供底层数据结构或额外组件的支持 ...

    ...

***

### 也许要扯些

[structc 环境搭建](./structc/README.md)

    - Winds 双击
        - 目前只维护 x64 的 Debug 和 Release
        - Best New CL build

    - Linux 搞起
        - Release : make
        - Clean   : make clean
        - Debug   : make D=-D_DEBUG

    - 工程目录简讲
        - main 目录
            - main_run.c 写业务代码
            - main_test.c 加 test目录下单测
        - test 目录
            - *_test.c -> void *_test(void) { ... }

***

### IDE 弱议

    Winds 还是万年不变的 Visual Studio Best Version

    Linux 抛砖引玉带大家配置哈 VS Code Best Version

    0. 安装 C/C++ Microsoft 插件
    
    1. F1 -> Edit Configurations -> c_cpp_properties.json
       Search Linux 部分添加如下内容
```json
            "name": "Linux",
            "includePath": [
                ...
                "${workspaceRoot}/structc/base",
                "${workspaceRoot}/structc/struct",
                "${workspaceRoot}/structc/system"
            ],
            "defines": [
                "__GNUC__",
                "_DEBUG"
            ],
```
    2. F5 -> launch.json
       按照规律改 program 项目生成 和 preLaunchTask 前置任务 
```json
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/Outs/main.exe",

            "preLaunchTask": "Debug",
```
    3. F5 -> tasks.json
       建立下面任务, 目前只使用了 Debug
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "type": "shell",
            "label": "Debug",
            "command": "make D=-D_DEBUG"
        }
    ]
}
```
    核心思路是基于 Makefile Debug 模式配置 VSCode C Studio.
    此刻就可以在 main.c 入口出发, 开启 F5 F10 F11 玩耍之路.
    
    比 vi + gdb 好那么一丢丢 ~

***

#### ~ * ~ 写个好手 ...

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

![江湖](./ryou.jpg)

***

    :)
