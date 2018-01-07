#include <rtree.h>

//
// struct $rtree 结构辅助操作宏
// r    : 当前结点
// p    : 父结点
// c    : 当前结点颜色, 1 表示 black, 0 表示 red
//
#define rtree_parent(r)      ((struct $rtree *)((r)->parentc & ~3))
#define rtree_color(r)       (1 & (r)->parent_color)
#define rtree_is_red(r)      (!rtree_color(r))
#define rtree_is_black(r)    rtree_color(r)
#define rtree_set_red(r)     (r)->parentc &= ~1
#define rtree_set_black(r)   (r)->parentc |= 1

inline void rtree_set_parent(struct $rtree * r, struct $rtree * p) {
    r->parentc = (r->parentc & 3) | (uintptr_t)p;
}

inline void rb_set_color(struct $rtree * r, int color) {
    r->parentc = (r->parentc & ~1) | (1 & color);
}

static inline int _rtree_default_cmp(const void * ln, const void * rn) {
    return (int)((intptr_t)ln - (intptr_t)rn);
}

//
// rtee_create - 创建一个红黑树结构
// fcmp     : 结点查找函数
// fnew     : 结点构造函数
// fdie     : 结点销毁函数
// return   : 返回构建红黑树
//
inline rtree_t 
rtree_create_(icmp_f fcmp, vnew_f fnew, node_f fdie) {
    rtree_t tree = malloc(sizeof *tree);
    tree->root = NULL;
    tree->fcmp = fcmp ? fcmp : _rtree_default_cmp;
    tree->fnew = fnew;
    tree->fdie = fdie;
    return tree;
}

// 详细删除操作, 存在爆栈风险
static void _rtree_delete(struct $rtree * root, node_f fdie) {
    if (root->left)
        _rtree_delete(root->left, fdie);
    if (root->right)
        _rtree_delete(root->right, fdie);
    fdie(root);
}

//
// rtree_delete - 红黑树销毁函数
// tree     : 待销毁的红黑树
// return   : void
//
inline void 
rtree_delete(rtree_t tree) {
    if (NULL == tree) return;
    if (tree->root && tree->fdie)
        _rtree_delete(tree->root, tree->fdie);

    tree->root = NULL;
    free(tree);
}

// 插入时候构造一个新结点
inline struct $rtree * rtree_new(rtree_t tree, void * pack) {
    struct $rtree * node = tree->fnew ? tree->fnew(pack) : pack;
    memset(node, 0, sizeof *node);
    return node;
}

