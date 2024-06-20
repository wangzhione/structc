#include "base.h"
#include "socket.h"

#include "config/config.h"

//
// CONFIG_PATH  - 配置文件路径
// LOGS_PATH    - 日志文件路径
// 
#define CONFIG_PATH             "config/config.json.conf"
#define LOGS_PATH               "logs/structc.log"

//
// init - 模块初始化
// return    : void
//
void init(int argc, char * argv[]) {
    UNUSED(argc); UNUSED(argv);

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
    // 不提倡系统 rand, 推荐 r_rand()
    srand(r_rand());

    // socket 模块初始化 ~
    socket_init();

    //
    // exec 业务 init
    // 微笑

    char path[BUFSIZ];

    // 一切皆有可能 🙂
    size_t n = getawd(path, LEN(path));
    // 配置模块初始化
    IF(!config_init(strcat(path, CONFIG_PATH)));

    // 日志模块初始化
    memcpy(path + n, LOGS_PATH, LEN(LOGS_PATH));
    fmkdir(path);
    EXTERN_RUN(log_init, path);
}
