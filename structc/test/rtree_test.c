#include <rtree.h>
#include <assext.h>

struct names {
    $RTREE

    char * name;
};

static inline struct names * _names_new(char * name) {
    struct names * node = malloc(sizeof(struct names));
    node->name = str_dup(name);
    return node;
}

static inline void _names_die(struct names * node) {
    // hook insert test
    printf("die -> name : %p | %s\n", node, node->name);
    free(node->name);
    free(node);
}

static inline int _names_cmp(struct names * left, struct names * right) {
    return strcmp(left->name, right->name);
}

static inline int _names_get(struct names * node, char * name) {
    return strcmp(node->name, name);
}

// names 测试打印处理
static void _names_print(struct names * root) {
    if (NULL == root) return;
    _names_print((struct names *)root->$node.left);
    printf("prt -> name : %s\n", root->name);
    _names_print((struct names *)root->$node.right);
}

//
// rtree test
//
void rtree_test(void) {
    rtree_t names = rtree_create(_names_cmp, _names_new, _names_die);

    // 插入数据
    rtree_insert(names, "aaa");
    rtree_insert(names, "bbb");
    rtree_insert(names, "ccc");
    rtree_insert(names, "正直");

    // 测试数据
    _names_print((struct names *)names->root);

    // 获取数据
    struct names node = { .name = "正直" };
    struct names * name = rtree_search(names, &node);
    CERR_IF(NULL == name);
    printf("get -> name : %p | %s\n", name, name->name);

    // 继续获取数据
    rtree_insert(names, "ddd");
    rtree_insert(names, "追求");

    // 继续查找
    names->fget = (icmp_f)_names_get;
    name = rtree_search(names, "追求");
    CERR_IF(NULL == name);
    printf("get -> name : %p | %s\n", name, name->name);

    // 删除数据
    rtree_remove(names, "bbb");

    rtree_delete(names);
}
