#include <alloc.h>
#include <stdio.h>

//
// 简单测试一下内存模块使用
void alloc_test(void) {
    size_t hoge = 9527;
    printf("hoge = %zu, sizoef(size_t) = %zu\n", hoge, sizeof(size_t));

    int * piyo = malloc(sizeof(int));
    printf("piyo = %p, %d.\n", piyo, *piyo);
    free(piyo);

    piyo = calloc(2, sizeof (int));
    printf("piyo = %p, %d.\n", piyo, *piyo);

    piyo = realloc(piyo, sizeof(int));
    printf("piyo = %p, %d.\n", piyo, *piyo);
    free(piyo);
}
