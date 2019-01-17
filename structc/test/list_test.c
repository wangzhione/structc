#include <list.h>

#define NAME_INT (64)

struct people {
$LIST   // 第一步: $LIST 嵌入第一行
    double age;
    char name[NAME_INT + 1];
};

// people_add - 默认年龄从小到大排序
inline static int people_add(const struct people * left, 
                             const struct people * node) {
    return (int)(left->age - node->age);
}

// people_each - 单纯的打印接口信息
inline static void people_each(struct people * node) {
    printf("age = %9.6lf, name = %s\n", node->age, node->name);
}

#define LEN(a) sizeof(a)/sizeof(*(a))

//
// list test demo
//
void list_test(void) {
    struct people * list = NULL; // 第二步: list 创建

    // people list 添加数据
    struct people ple[5];
    for (int i = 0; i < LEN(ple); ++i) {
        ple[i].age = rand() % 100 + rand() * 1.0 / rand();
        snprintf(ple[i].name, LEN(ple[i].name), "ple_%d", i);
        list_add(&list, people_add, ple + i);
    }

    // 这里打印数据
    list_each(list, people_each);
}
