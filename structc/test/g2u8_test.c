#include <g2u8.h>
#include <strext.h>

// g2u8_test gbk utf 编码测试
void g2u8_test(void) {
    // name 地址足够大
    char name[BUFSIZ] = "孩子 - http://music.163.com/m/song?id=536890393&userid=16529894";

    printf("[%s] is utf8 = %d.\n", name, isu8s(name));
    g2u8(name, sizeof name);
    printf("[%s] is utf8 = %d.\n", name, isu8s(name));
    u82g(name);
    printf("[%s] is utf8 = %d.\n", name, isu8s(name));

    // utf8 -> gbk 
    char * u8s = u8"你好吗, 😀";
    puts(u8s);
    char * gs = u82gs(u8s);
    puts(gs);
    char * n8s = g2u8s(gs);
    free(gs);
    puts(n8s);
    if (strncmp(u8s, n8s, strlen(u8s))) {
        puts("error parse ... ");
    }
    free(n8s);

    puts("success");
}
