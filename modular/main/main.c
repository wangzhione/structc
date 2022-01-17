#include "base.h"
#include "socket.h"

//
// main_init - 模块初始化
// return    : void
//
static void main_init(void) {
    // Now 'timezome' global is populated. Obtain timezone and daylight info. 
    tzset();

# if defined(_WIN32) && defined(_MSC_VER)
    // 手动设置 window console 编码为 UTF-8
    // 65001    UTF-8 代码页
    // 950      繁体中文
    // 936      简体中文 默认 GBK
    // 437      MS DOS 美国英语
    int res = system("chcp 65001");
    assert(res >= 0);
# endif

    // thread 模块初始化
    EXTERN_RUN(pthread_init);

    // 随机函数 init 😂
    struct timespec s; 
    timespec_get(&s, TIME_UTC);
    EXTERN_RUN(r_init, s.tv_nsec+s.tv_sec);

    // socket 模块初始化 ~
    socket_init();

    // file 模块初始化
    EXTERN_RUN(file_init);
}

//
// main     - 随风奔跑, 追逐闪电 ~ 
// return   : 状态码 0 is success
//
int main(void) {
    // 系统初始化 ~ 虎
    main_init();
    
    // 业务初始化 ~ 犬
    EXTERN_RUN(exec_init);

    // main test 单元测试消除 ~ 魅
    // Release build: make D=-DNDEBUG
    //
#if !defined(NDEBUG) || defined(_DEBUG)
    EXTERN_RUN(test);
#endif

    // ... 执行业务主 ~ 龍
    //
    EXTERN_RUN(exec);

    exit(EXIT_SUCCESS);
}
