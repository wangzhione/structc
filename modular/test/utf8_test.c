#include <utf8.h>
#include <base.h>
#include <utf8$table.h>

// utf8_test gbk utf 编码测试
void utf8_test(void) {
    // name 地址足够大
    char name[BUFSIZ] = "孩子 - http://music.163.com/m/song?id=536890393&userid=16529894";

    printf("[%s] is utf8 = %d.\n", name, isu8s(name));
    g2u8(name, sizeof name);
    printf("[%s] is utf8 = %d.\n", name, isu8s(name));
    u82g(name);
    printf("[%s] is utf8 = %d.\n", name, isu8s(name));

    // utf8 -> gbk 
    char * u8s = u8"你好吗, 😀";
    printf("u8s -> %d\n", isu8s(u8s));

    puts("success");
}

void utf8_table_file_test(void) {
    int i;
    const char * filename =  "utf8$table.h";

    FILE * stream = fopen(filename, "w+");
    IF(!stream);

    fprintf(stream, "#ifndef _STRUCTC_SYSTEM_UTF8$TABLE_H"           "\n");
    fprintf(stream, "#define _STRUCTC_SYSTEM_UTF8$TABLE_H"           "\n");
    fprintf(stream,                                                  "\n");
    fprintf(stream,"#include <stdint.h>"                             "\n");
    fprintf(stream,                                                  "\n");
    fprintf(stream, "// unicode to gbk 编码映射表"                   "\n");
    fprintf(stream, "static const uint16_t unicode_gbk_table[] = { " "\n");

    for (i = 1; i <= LEN(unicode_gbk_table); i++) {
        if (i % 10 == 1)
            fprintf(stream, "    ");

        fprintf(stream, "0x%-8x, ", unicode_gbk_table[i-1]);

        if (i % 10 == 0)
            fprintf(stream, "\n");
    }
    if (i % 10 != 0)
        fprintf(stream, "\n");

    fprintf(stream, "};"                                              "\n");
    fprintf(stream,                                                   "\n");
    fprintf(stream, "// gbk to utf8 编码映射表"                       "\n");
    fprintf(stream, "static const unsigned    gbk_utf8_table[] = { "  "\n");

    for (i = 1; i <= LEN(gbk_utf8_table); i++) {
        if (i % 10 == 1)
            fprintf(stream, "    ");

        fprintf(stream, "0x%08x, ", gbk_utf8_table[i-1]);

        if (i % 10 == 0)
            fprintf(stream, "\n");
    }
    if (i % 10 != 0)
        fprintf(stream, "\n");

    fprintf(stream, "};"                                             "\n");
    fprintf(stream,                                                  "\n");
    fprintf(stream, "#endif//_STRUCTC_SYSTEM_UTF8$TABLE_H"           "\n");

    fclose(stream);
}
