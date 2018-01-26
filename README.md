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

    - winds 双击
        - 目前只维护 x64 的 Debug 和 Release
        - Best New CL build

    - linux 搞起
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
