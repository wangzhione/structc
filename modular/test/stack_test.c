#include <stack.h>

void stack_test(void) {
    struct stack s = stack_create();

    char * str = nullptr;
    stack_push(&s, ++str);
    stack_push(&s, ++str);
    stack_push(&s, ++str);

    // 数据输出
    while (stack_exist(&s)) {
        printf("now = %p\n", stack_top(&s));
        stack_popped(&s);
    }

    stack_push(&s, ++str);
    stack_push(&s, ++str);

    while (stack_exist(&s)) {
        printf("now = %p\n", stack_pop(&s));
    }

    stack_release(&s);
}
