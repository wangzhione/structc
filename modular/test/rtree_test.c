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

// names_search - names rtree search
inline struct names * names_search(rtree_t tree, char * name) {
    struct names key = { .name = name };
    return rtree_search(tree, &key);
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
    rtree_t tree = rtree_create(names_cmp, names_die);

    // 插入数据
    rtree_insert(tree, names_new("aaa"));
    rtree_insert(tree, names_new("bbb"));
    rtree_insert(tree, names_new("ccc"));
    rtree_insert(tree, names_new("正直"));

    // 测试数据
    names_print((struct names *)tree->root);

    // 获取数据
    struct names * name = names_search(tree, "正直");
    IF(!name);
    printf("get->name: %p | %s\n", name, name->name);

    // 继续获取数据
    rtree_insert(tree, names_new("ddd"));
    rtree_insert(tree, names_new("追求"));

    // 继续查找
    name = names_search(tree, "追求");
    IF(!name);
    printf("get->name: %p | %s\n", name, name->name);

    // 删除数据
    name = names_search(tree, "bbb");
    if (name) {
        printf("name = %p, %s\n", name, name->name);
        rtree_remove(tree, name);
    }

    puts("rtree_delete");
    rtree_delete(tree);
}
