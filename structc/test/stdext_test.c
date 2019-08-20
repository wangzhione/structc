#include <struct.h>
#include <stdext.h>

void stdext_test(void) {
    const char * path = "README.md";

    time_t mt = st_mtime(path);
    printf("st_mtime mt = %lld\n", mt);

    // 从上面最近修改时间, 来文件大小测试试试
    FILE * txt = fopen(path, "rb");
    IF(!txt);
    fseek(txt, 0, SEEK_END);
    int64_t size = ftell(txt);
    fclose(txt);

    printf("fseek ftell size = %lld\n", size);

    size = st_size(path);
    printf("st_size size = %lld\n", size);

    // 获取程序运行目录结构
    char buf[BUFSIZ];
    puts(getcwd(buf, BUFSIZ));
}
