#include <file.h>

// mkdirs_test 单纯的函数测试
inline mkdirs_test(const char *path) {
    int ret = mkdirs(path);
    printf("mkdir -p %s -> %d\n", path, ret);
}

// removes_test - 单纯测试删除操作
inline removes_test(const char * path) {
    int ret = removes(path);
    printf("rm -rf '%s' -> %d\n", path, ret);
}

//
// file 构建
//
void file_test(void) {
    mkdirs_test("logs/heoo");
    mkdirs_test("logs/piyo/piyo");
    mkdirs_test("logs/heoos/heoos/");
    mkdirs_test("logs/heoos/heoos/gghh\\gggs/g/");
    mkdirs_test("logs/heos/heos/gghhs\\ggg/s/\\");

    removes_test("logs/heoos/");
    removes_test("logs/heoos/");
    // 文件句柄占用 winds 无法删除
    // removes_test("logs/");
}
