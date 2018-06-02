#include <list.h>
#include <chead.h>

#define INT_NAME (64)

struct peoples {
    $LIST

    double age;
    char name[INT_NAME + 1];
};

// _peoples_add : 默认年龄从小到大排序, 并且获取
inline static int peoples_add(struct peoples * left, struct peoples * node) {
    return (int)(left->age - node->age);
}

// _peoples_each : 单纯的打印接口信息
inline static void peoples_each(struct peoples * node) {
    printf("age = %9.6lf, name = %s\n", node->age, node->name);
}

//
// list test demo
//
void list_test(void) {
    void * peops = NULL;

    // 这里添加数据
    struct peoples peop[5];
    for (int i = 0; i < LEN(peop); ++i) {
        peop[i].age = rand() % 100 + rand() * 1.0 / rand();
        snprintf(peop[i].name, LEN(peop[i].name), "peop_%d", i);
        list_add(peops, peoples_add, peop + i);
    }

    // 这里打印数据
    list_each(peops, peoples_each);
}
