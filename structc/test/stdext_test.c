#include <stdext.h>

//
// 简单测试一下内存模块使用
//
void stdext_test(void) {
    int * piyo = malloc(sizeof(int));
    printf("piyo = %p, %d.\n", piyo, *piyo);
    free(piyo);

    piyo = calloc(2, sizeof (int));
    printf("piyo = %p, %d.\n", piyo, *piyo);

    piyo = realloc(piyo, sizeof(int));
    printf("piyo = %p, %d.\n", piyo, *piyo);
    free(piyo);
}
