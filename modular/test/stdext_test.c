#include <struct.h>
#include <stdext.h>
#include <msg.h>

PACKED(struct sdshdr5 {
    unsigned char flags; /* 3 lsb of type, and 5 msb of string length */
    char buf[];
});

// small - 转本地字节序(小端)
inline uint32_t small(uint32_t x) {
# ifdef ISBIG
    uint8_t t, * p = (uint8_t *) &x;

    t = p[0]; p[0] = p[3]; p[3] = t;
    t = p[1]; p[1] = p[2]; p[2] = t;
# endif
    return x;
}

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
