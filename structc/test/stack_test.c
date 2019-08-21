#include <stack.h>

void stack_test(void) {
    stack_create(s);

    char * str = NULL;
    stack_push(s, ++str);
    stack_push(s, ++str);
    stack_push(s, ++str);

    // 数据输出
    char * now;
    while ((now = stack_pop(s))) {
        printf("now = %p\n", now);
    }

    stack_push(s, ++str);
    stack_push(s, ++str);

    while ((now = stack_pop(s))) {
        printf("now = %p\n", now);
    }

    stack_delete(s, NULL);
}