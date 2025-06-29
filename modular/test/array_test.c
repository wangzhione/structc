#include <array.h>

// array_test - array test
void array_test(void) {
    // 构建一个在栈上的动态数组
    struct array a;

    IF (array_init(&a, sizeof(double)) == false);

    // 开始处理数据, 内存不足返回 nullptr 程序会崩溃
    *(double *)array_push(&a) = 1.1234;
    *(double *)array_push(&a) = 2.2345;
    *(double *)array_push(&a) = 4.9876;

    // 输出数据
    printf("v = %lf\n", *(double *)array_pop(&a));
    printf("v = %lf\n", *(double *)array_pop(&a));
    printf("v = %lf\n", *(double *)array_pop(&a));

    array_release(&a);
}
