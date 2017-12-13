#include <list.h>
#include <chead.h>

#define _INT_NAME (64)

struct peoples {
    $LIST

    double age;
    char name[_INT_NAME + 1];
};

// _peoples_add : 默认年龄从小到大排序, 并且获取
static inline int _peoples_add(struct peoples * left, struct peoples * node) {
    return left->age - node->age;
}

// _peoples_each : 单纯的打印接口信息
static void _peoples_each(struct peoples * node) {
    printf("age = %lf, name = %s\n", node->age, node->name);
}

//
// list test demo
//
void list_test(void) {
    list_t peops = list_create(_peoples_add, _peoples_add);

    // 这里添加数据
    struct peoples peop[5];
    for (int i = 0; i < LEN(peop); ++i) {
        peop[i].age = rand() % 100 + rand() * 1.0 / rand();
        snprintf(peop[i].name, LEN(peop[i].name), "peop_%d", i);
        list_add(peops, peop + i);
    }

    // 这里打印数据
    list_each(peops, _peoples_each);

    list_delete(peops);
}
