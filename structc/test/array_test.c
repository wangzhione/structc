#include <array.h>

// array_test - array test
void array_test(void) {
    // 构建一个在栈上的动态数组
    ARRAY_CREATE(double, a);

    // 开始处理数据
    *(double *)array_push(a) = 1.1234;
    *(double *)array_push(a) = 2.2345;
    *(double *)array_push(a) = 4.9876;

    // 输出数据
    printf("v = %lf\n", *(double *)array_pop(a));
    printf("v = %lf\n", *(double *)array_pop(a));
    printf("v = %lf\n", *(double *)array_pop(a));

    ARRAY_DELETE(a);
}
