#include <file.h>
#include <times.h>

// mkdirs_test 单纯的函数测试
inline void mkdirs_test(const char *path) {
    int ret = mkdirs(path);
    printf("mkdir -p %s -> %d\n", path, ret);
}

// removes_test - 单纯测试删除操作
inline void removes_test(const char * path) {
    int ret = removes(path);
    printf("rm -rf '%s' -> %d\n", path, ret);
}

#define CONF_STR "conf/config.conf"

// conf_update - 简单测试代码
static void conf_update(FILE * c, void * arg) {
    printf("c = %p, arg = %p\n", c, arg);
}

//
// file 构建
//
void file_test(void) {
    mkdirs_test("logs/heoos");
    mkdirs_test("logs/heoos/heoos/");
    mkdirs_test("logs/heoos/heoos/gghh\\gggs/g/");

    removes_test("logs/heoos/");
    removes_test("logs/heoos/");
    // 文件句柄占用 winds 无法删除
    // removes_test("logs/");

    // 开始注册一个
    file_set(CONF_STR, conf_update, NULL);

    // 定时刷新, 凡事追求 61分.
    for (int i = 0; i < 61; ++i) {
        puts("conf start");
        file_update();
        // 1s 一测试
        msleep(1000);
        puts("conf e n d");
    }

    // 销毁操作丢给上帝
}
