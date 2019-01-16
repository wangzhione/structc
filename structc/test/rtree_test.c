#include <rtree.h>

struct names {
$RTREE
    char * name;
};

inline static struct names * names_new(char * name) {
    struct names * node = malloc(sizeof(struct names));
    node->name = strdup(name);
    return node;
}

inline static void names_die(struct names * node) {
    // hook insert test
    printf("die -> name : %p | %s\n", node, node->name);
    free(node->name);
    free(node);
}

inline static int names_cmp(struct names * left, struct names * right) {
    return strcmp(left->name, right->name);
}

inline static int names_get(struct names * node, char * name) {
    return strcmp(node->name, name);
}

// names_print - 测试打印处理
void names_print(struct names * root) {
    if (root) {
        names_print((struct names *)root->$node.left);
        printf("root->name : %s\n", root->name);
        names_print((struct names *)root->$node.right);
    }
}

//
// rtree test
//
void rtree_test(void) {
    rtree_t names = rtree_create(names_cmp, names_new, names_die);

    // 插入数据
    rtree_insert(names, "aaa");
    rtree_insert(names, "bbb");
    rtree_insert(names, "ccc");
    rtree_insert(names, "正直");

    // 测试数据
    names_print((struct names *)names->root);

    // 获取数据
    struct names node = { .name = "正直" };
    struct names * name = rtree_search(names, &node);
    IF(NULL == name);
    printf("get->name: %p | %s\n", name, name->name);

    // 继续获取数据
    rtree_insert(names, "ddd");
    rtree_insert(names, "追求");

    // 继续查找
    names->fget = (cmp_f)names_get;
    name = rtree_search(names, "追求");
    IF(NULL == name);
    printf("get->name: %p | %s\n", name, name->name);

    // 删除数据
    rtree_remove(names, "bbb");

    rtree_delete(names);
}
