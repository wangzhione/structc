#include <array.h>

//
// array test
//
void array_test(void) {
    // 构建一个在栈上的动态数组
    ARRAY_CREATE(double, as);

    // 开始处理数据
    *(double *)array_push(as) = 1.1234;

    *(double *)array_push(as) = 2.2345;

    *(double *)array_push(as) = 4.9876;

    // 输出数据
    printf("a = %lf\n", *(double *)array_pop(as));

    printf("a = %lf\n", *(double *)array_pop(as));

    printf("a = %lf\n", *(double *)array_pop(as));

    ARRAY_DELETE(as);
}