#include <stack.h>

void stack_test(void) {
    struct stack s = stack_create();

    char * str = NULL;
    stack_push(&s, ++str);
    stack_push(&s, ++str);
    stack_push(&s, ++str);

    // 数据输出
    for (char * now; (now = stack_top(&s)); stack_popped(&s))
        printf("now = %p\n", now);

    stack_push(&s, ++str);
    stack_push(&s, ++str);

    for (char * now; (now = stack_top(&s)); stack_popped(&s))
        printf("now = %p\n", now);

    stack_release(&s);
}
