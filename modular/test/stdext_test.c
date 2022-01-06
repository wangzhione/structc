#include <struct.h>
#include <stdext.h>
#include <system.h>
#include <msg.h>

PACKED(struct sdshdr5 {
    unsigned char flags; /* 3 lsb of type, and 5 msb of string length */
    char buf[];
});

void stdext_test(void) {
    const char * path = "README.md";

    time_t mt = fmtime(path);
    printf("st_mtime mt = %"PRId64"\n", mt);

    // 从上面最近修改时间, 来文件大小测试试试
    FILE * txt = fopen(path, "rb");
    IF(!txt);
    fseek(txt, 0, SEEK_END);
    int64_t size = ftell(txt);
    fclose(txt);

    printf("fseek ftell size = %"PRId64"\n", size);

    size = fsize(path);
    printf("st_size size = %"PRId64"\n", size);

    // 获取程序运行目录结构
    char buf[BUFSIZ];
    puts(getcwd(buf, BUFSIZ));

    uint32_t x = 0x12345678;
    printf("x = 0x%x -> 0x%x\n", x, small(x));
}
