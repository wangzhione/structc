#include <utf8.h>
#include <strext.h>

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
