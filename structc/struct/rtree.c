#include "rtree.h"

//
// struct $rtree 结构辅助操作宏
// r    : 当前结点
// p    : 父结点
// c    : 当前结点颜色, 1 is black, 0 is red
//
#define rtree_parent(r)      ((struct $rtree *)((r)->parentc & ~1))
#define rtree_color(r)       ((r)->parentc & 1)
#define rtree_is_red(r)      (!rtree_color(r))
#define rtree_is_black(r)    rtree_color(r)
#define rtree_set_red(r)     (r)->parentc &= ~1
#define rtree_set_black(r)   (r)->parentc |=  1

inline void rtree_set_parent(struct $rtree * r, struct $rtree * p) {
    r->parentc = (r->parentc & 1) | (uintptr_t)p;
}

inline void rtree_set_color(struct $rtree * r, int color) {
    r->parentc = (r->parentc & ~1) | (1 & color);
}

inline static int rtree_cmp_default(const void * l, const void * r) {
    return (int)((intptr_t)l - (intptr_t)r);
}

//
// rtee_create - 创建一个红黑树对象
// fcmp     :  cmp_f 结点的比较行为
// fdie     : node_f 结点的销毁行为
// return   : 返回构建红黑树对象
//
inline rtree_t 
rtree_create(void * fcmp, void * fdie) {
    rtree_t tree = malloc(sizeof *tree);
    tree->root = NULL;
    tree->fcmp = fcmp ? fcmp : rtree_cmp_default;
    tree->fdie = fdie;
    return tree;
}

// rtree_die - 后序删除树结点
static void rtree_die(struct $rtree * root, node_f fdie) {
    struct $rtree * pre = NULL;
    struct stack s; stack_init(&s);
    stack_push(&s, root);
    do {
        struct $rtree * cur = stack_top(&s);
        if ((!cur->left && !cur->right) 
         || ((cur->left == pre || cur->right == pre) && pre)) {
            fdie(pre = cur);
            stack_pop(&s);
        } else {
            if (cur->right)
                stack_push(&s, cur->right);
            if (cur->left)
                stack_push(&s, cur->left);
        }
    } while (!stack_empty(&s));
    stack_free(&s);
}

//
// rtree_delete - 红黑树删除函数
// tree     : 待删除的红黑树
// return   : void
//
inline void 
rtree_delete(rtree_t tree) {
    if (tree) {
        if (tree->root && tree->fdie)
            rtree_die(tree->root, tree->fdie);
        tree->root = NULL;
        free(tree);
    }
}

//
// rtree_search - 红黑树查找函数
// tree     : 待查找的红黑树结构
// pack     : 树结点, fcmp(x, pack)
// return   : 返回查找的结点
//
void * 
rtree_search(rtree_t tree, void * pack) {
    cmp_f fcmp = tree->fcmp;
    struct $rtree * node = tree->root;
    while (node) {
        int diff = fcmp(node, pack);
        if (diff == 0)
            break;
        //
        // tree left less, right greater 
        //
        node = diff > 0 ? node->left : node->right;
    }
    return node;
}

/*
 红黑树性质

    性质1: 结点是红色或黑色
    性质2: 根是黑色
    性质3: 所有 NULL 叶子都是黑色
    性质4: 从任一结点到其每个叶子的所有简单路径都包含相同数目的黑色结点
    性质5: 从每个叶子到根的所有路径上不能有两个连续的红色结点
 
 详细的可以查看, 下面博主的红黑树系列文章
    https://www.cnblogs.com/skywang12345/p/3624177.html

 */

/* 
 * 对红黑树的结点 [x] 进行左旋转
 *
 * 左旋示意图 (对结点 x 进行左旋):
 * 
 *       px                             px
 *      /                              /
 *     x                              y
 *    /  \       --- (左旋) -->      / \
 *   lx   y                         x   ry
 *      /   \                     /   \
 *     ly   ry                   lx   ly  
 *
 */
static void rtree_left_rotate(rtree_t tree, struct $rtree * x) {
    // 设置 [x] 的右孩子为 [y]
    struct $rtree * y = x->right;
    struct $rtree * xparent = rtree_parent(x);

    // 将 [y的左孩子] 设为 [x的右孩子]；
    x->right = y->left;
    // 如果 y的左孩子 非空，将 [x] 设为 [y的左孩子的父亲]
    if (y->left)
        rtree_set_parent(y->left, x);

    // 将 [x的父亲] 设为 [y的父亲]
    rtree_set_parent(y, xparent);

    if (!xparent) {
        // 如果 [x的父亲] 是空结点, 则将 [y] 设为根结点
        tree->root = y;
    } else {
        if (xparent->left == x) {
            // 如果 [x] 是它父结点的左孩子, 则将 [y] 设为 [x的父结点的左孩子]
            xparent->left = y;
        } else {
            // 如果 [x] 是它父结点的左孩子, 则将 [y] 设为 [x的父结点的左孩子]
            xparent->right = y;
        }
    }

    // 将 [x] 设为 [y的左孩子]
    y->left = x;
    // 将 [x的父结点] 设为 [y]
    rtree_set_parent(x, y);
}

/* 
 * 对红黑树的结点 [y] 进行右旋转
 *
 * 右旋示意图(对结点y进行左旋)：
 * 
 *         py                            py
 *        /                             /
 *       y                             x                  
 *      /  \      --- (右旋) -->      /  \
 *     x   ry                        lx   y  
 *    / \                                / \
 *   lx  rx                             rx  ry
 * 
 */
static void rtree_right_rotate(rtree_t tree, struct $rtree * y) {
    // 设置 [x] 是当前结点的左孩子。
    struct $rtree * x = y->left;
    struct $rtree * yparent = rtree_parent(y);

    // 将 [x的右孩子] 设为 [y的左孩子]
    y->left = x->right;
    // 如果 x的右孩子 不为空的话，将 [y] 设为 [x的右孩子的父亲]
    if (x->right)
        rtree_set_parent(x->right, y);

    // 将 [y的父亲] 设为 [x的父亲]
    rtree_set_parent(x, yparent);
    if (!yparent) {
        // 如果 [y的父亲] 是空结点, 则将 [x] 设为根结点
        tree->root = x;
    } else {
        if (y == yparent->right) {
            // 如果 [y] 是它父结点的右孩子, 则将 [x] 设为 [y的父结点的右孩子]
            yparent->right = x;
        } else {
            // 如果 [y] 是它父结点的左孩子, 将 [x] 设为 [x的父结点的左孩子]
            yparent->left = x;
        }
    }

    // 将 [y] 设为 [x的右孩子]
    x->right = y;
    // 将 [y的父结点] 设为 [x]
    rtree_set_parent(y, x);
}

/*
 * 红黑树插入修正函数
 *
 * 在向红黑树中插入结点之后(失去平衡), 再调用该函数.
 * 目的是将它重新塑造成一颗红黑树.
 *
 * 参数说明:
 *     tree 红黑树的根
 *     node 插入的结点        // 对应 <<算法导论>> 中的 z
 */
static void rtree_insert_fixup(rtree_t tree, struct $rtree * node) {
    struct $rtree * parent, * gparent, * uncle;

    // 若 [父结点] 存在，并且 [父结点] 的颜色是红色
    while ((parent = rtree_parent(node)) && rtree_is_red(parent)) {
        gparent = rtree_parent(parent);

        //若 [父结点] 是 [祖父结点的左孩子]
        if (parent == gparent->left) {
            // Case 1 条件: 叔叔结点是红色
            uncle = gparent->right;
            if (uncle && rtree_is_red(uncle)) {
                rtree_set_black(uncle);
                rtree_set_black(parent);
                rtree_set_red(gparent);
                node = gparent;
                continue;
            }

            // Case 2 条件: 叔叔是黑色, 且当前结点是右孩子
            if (parent->right == node) {
                rtree_left_rotate(tree, parent);
                uncle = parent;
                parent = node;
                node = uncle;
            }

            // Case 3 条件: 叔叔是黑色, 且当前结点是左孩子
            rtree_set_black(parent);
            rtree_set_red(gparent);
            rtree_right_rotate(tree, gparent);
        } else { // 若 [z的父结点] 是 [z的祖父结点的右孩子]
            // Case 1 条件: 叔叔结点是红色
            uncle = gparent->left;
            if (uncle && rtree_is_red(uncle)) {
                rtree_set_black(uncle);
                rtree_set_black(parent);
                rtree_set_red(gparent);
                node = gparent;
                continue;
            }

            // Case 2 条件: 叔叔是黑色, 且当前结点是左孩子
            if (parent->left == node) {
                rtree_right_rotate(tree, parent);
                uncle = parent;
                parent = node;
                node = uncle;
            }

            // Case 3 条件: 叔叔是黑色, 且当前结点是右孩子
            rtree_set_black(parent);
            rtree_set_red(gparent);
            rtree_left_rotate(tree, gparent);
        }
    }

    // 将根结点设为黑色
    rtree_set_black(tree->root);
}

//
// rtree_insert - 红黑树中插入结点 fnew(pack)
// tree     : 红黑树结构
// pack     : 待插入基础结构
// return   : void
//
void 
rtree_insert(rtree_t tree, void * pack) {
    cmp_f fcmp = tree->fcmp;
    struct $rtree * x = tree->root, * y = NULL;
    // 1. 构造插入结点, 并设置结点的颜色为红色
    struct $rtree * node = memset(pack, 0, sizeof(struct $rtree));

    // 2. 将红黑树当作一颗二叉查找树, 将结点添加到二叉查找树中. 默认 从小到大
    while (x) {
        y = x;
        if (fcmp(x, node) > 0)
            x = x->left;
        else
            x = x->right;
    }
    rtree_set_parent(node, y);

    if (!y) {
        // 情况 1: 若 y是空结点, 则将 node设为根
        tree->root = node;
    } else {
        if (fcmp(y, node) > 0) {
            // 情况 2: 
            // 若 "node所包含的值" < "y所包含的值", 则将 [node] 设为 [y的左孩子]
            y->left = node;
        } else {
            // 情况 3: 
            // 若 "node所包含的值" >= "y所包含的值", 将 [node] 设为 [y的右孩子] 
            y->right = node;
        }
    }

    // 3. 将它重新修正为一颗二叉查找树
    rtree_insert_fixup(tree, node);
}

/*
 * 红黑树删除修正函数
 *
 * 在从红黑树中删除插入结点之后(红黑树失去平衡), 再调用该函数.
 * 目的是将它重新塑造成一颗红黑树.
 *
 * 参数说明:
 *     tree 红黑树的根
 *     node 待修正的结点
 */
static void rtree_remove_fixup(rtree_t tree, 
                               struct $rtree * node, 
                               struct $rtree * parent) {
    struct $rtree * other;

    while ((!node || rtree_is_black(node)) && node != tree->root) {
        if (parent->left == node) {
            other = parent->right;
            if (rtree_is_red(other)) {
                // Case 1: x的兄弟 w 是红色的  
                rtree_set_black(other);
                rtree_set_red(parent);
                rtree_left_rotate(tree, parent);
                other = parent->right;
            }
            if ((!other->left || rtree_is_black(other->left)) &&
                (!other->right || rtree_is_black(other->right))) {
                // Case 2: x的兄弟 w 是黑色, 且 w的俩个孩子也都是黑色的  
                rtree_set_red(other);
                node = parent;
                parent = rtree_parent(node);
            } else {
                if (!other->right || rtree_is_black(other->right)) {
                    // Case 3: x的兄弟 w 是黑色的, 并且 w的左孩子是红色, 右孩子为黑色  
                    rtree_set_black(other->left);
                    rtree_set_red(other);
                    rtree_right_rotate(tree, other);
                    other = parent->right;
                }
                // Case 4: x的兄弟 w 是黑色的, 并且 w的右孩子是红色的, 左孩子任意颜色
                rtree_set_color(other, rtree_color(parent));
                rtree_set_black(parent);
                rtree_set_black(other->right);
                rtree_left_rotate(tree, parent);
                node = tree->root;
                break;
            }
        } else {
            other = parent->left;
            if (rtree_is_red(other)) {
                // Case 1: x 的兄弟 w 是红色的  
                rtree_set_black(other);
                rtree_set_red(parent);
                rtree_right_rotate(tree, parent);
                other = parent->left;
            }
            if ((!other->left || rtree_is_black(other->left)) &&
                (!other->right || rtree_is_black(other->right))) {
                // Case 2: x 的兄弟 w 是黑色, 且 w的俩个孩子 也都是黑色的  
                rtree_set_red(other);
                node = parent;
                parent = rtree_parent(node);
            } else {
                if (!other->left || rtree_is_black(other->left)) {
                    // Case 3: x的兄弟 w 是黑色的, 并且 w的左孩子是红色, 右孩子为黑色
                    rtree_set_black(other->right);
                    rtree_set_red(other);
                    rtree_left_rotate(tree, other);
                    other = parent->left;
                }
                // Case 4: x的兄弟 w 是黑色的, 并且 w的右孩子是红色的, 左孩子任意颜色.
                rtree_set_color(other, rtree_color(parent));
                rtree_set_black(parent);
                rtree_set_black(other->left);
                rtree_right_rotate(tree, parent);
                node = tree->root;
                break;
            }
        }
    }
    if (node) rtree_set_black(node);
}

//
// rtree_remove - 红黑树中删除结点
// tree     : 红黑树结构
// pack     : 此树中待删除结点
// return   : void
//
void 
rtree_remove(rtree_t tree, void * pack) {
    int color;
    struct $rtree * child, * parent, * node = pack;

    // 被删除结点的 "左右孩子都不为空" 的情况
    if (node->left && node->right) {
        // 被删结点的后继结点. (称为 "取代结点")
        // 用它来取代 "被删结点" 的位置, 然后再将 "被删结点" 去掉
        struct $rtree * replace = node;

        // 获取后继结点
        replace = replace->right;
        while (replace->left)
            replace = replace->left;

        // "node结点" 是根结点, 更新根结点
        if (!(parent = rtree_parent(node)))
            tree->root = replace;
        else {
            // "node结点" 不是根结点(只有根结点不存在父结点)
            if (parent->left == node)
                parent->left = replace;
            else
                parent->right = replace;
        }

        // child 是 "取代结点" 的右孩子, 也是需要 "调整的结点"
        // "取代结点" 肯定不存在左孩子! 因为它是一个后继结点
        child = replace->right;
        parent = rtree_parent(replace);
        // 保存 "取代结点" 的颜色
        color = rtree_color(replace);

        // "被删除结点" 是 "它的后继结点的父结点"
        if (parent == node)
            parent = replace; 
        else {
            // child不为空
            if (child)
                rtree_set_parent(child, parent);
            parent->left = child;

            replace->right = node->right;
            rtree_set_parent(node->right, replace);
        }

        rtree_set_parent(replace, rtree_parent(node));
        rtree_set_color(replace, rtree_color(node));
        replace->left = node->left;
        rtree_set_parent(node->left, replace);

        goto ret_out;
    }

    if (node->left)
        child = node->left;
    else 
        child = node->right;

    parent = rtree_parent(node);
    // 保存 "取代结点" 的颜色
    color = rtree_color(node);

    if (child)
        rtree_set_parent(child, parent);

    // "node结点" 不是根结点
    if (!parent)
        tree->root = child;
    else {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    }

ret_out:
    if (color) // 黑色结点重新调整关系, 并销毁结点操作
        rtree_remove_fixup(tree, child, parent);
    if (tree->fdie)
        tree->fdie(node);
}
