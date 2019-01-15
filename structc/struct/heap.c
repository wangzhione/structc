#include "heap.h"

#define HEAP_UINT       (1<<5u)

struct heap {
    cmp_f   fcmp;       // 比较行为
    unsigned len;       // heap 长度
    unsigned cap;       // heap 容量
    void ** data;       // 数据节点数组
};

// heap_expand - 添加节点扩容
inline void heap_expand(struct heap * h) {
    if (h->len >= h->cap) {
        h->data = realloc(h->data, h->cap<<=1);
        assert(h->data);
    }
}

//
// heap_create - 创建符合规则的堆
// fcmp     : 比较行为, 规则 fcmp() <= 0
// return   : 返回创建好的堆对象
//
inline heap_t 
heap_create(cmp_f fcmp) {
    struct heap * h = malloc(sizeof(struct heap));
    assert(h && fcmp);
    h->fcmp = fcmp;
    h->len = 0;
    h->cap = HEAP_UINT;
    h->data = malloc(sizeof(void *) * HEAP_UINT);
    assert(h->data && HEAP_UINT > 0);
    return h;
}

//
// heap_delete - 销毁堆
// h        : 堆对象
// fdie     : 销毁行为, 默认 NULL
// return   : void
//
void 
heap_delete(heap_t h, node_f fdie) {
    if (NULL == h || h->data == NULL) return;
    if (fdie && h->len > 0)
        for (unsigned i = 0; i < h->len; ++i)
            fdie(h->data[i]);
    free(h->data);
    h->data = NULL;
    h->len = 0;
    free(h);
}

// down - 堆节点下沉, 从上到下沉一遍
static void down(cmp_f fcmp, void * data[], unsigned len, unsigned x) {
    void * m = data[x];
    for (unsigned i = x * 2 + 1; i < len; i = x * 2 + 1) {
        if (i + 1 < len && fcmp(data[i+1], data[i]) < 0)
            ++i;
        if (fcmp(m, data[i]) <= 0)
            break;
        data[x] = data[i];
        x = i;
    }
    data[x] = m;
}

// up - 堆节点上浮, 从下到上浮一遍
static void up(cmp_f fcmp, void * node, void * data[], unsigned x) {
    while (x > 0) {
        void * m = data[(x-1)>>1];
        if (fcmp(m, node) <= 0)
            break;
        data[x] = m;
        x = (x-1)>>1;
    }
    data[x] = node;
}

//
// heap_insert - 堆插入数据
// h        : 堆对象
// node     : 操作对象
// return   : void
//
inline void 
heap_insert(heap_t h, void * node) {
    heap_expand(h);
    up(h->fcmp, node, h->data, h->len++);
}

//
// heap_remove - 堆删除数据
// h        : 堆对象
// arg      : 操作参数
// fcmp     : 比较行为, 规则 fcmp() == 0
// return   : 找到的堆节点
//
void * 
heap_remove(heap_t h, void * arg, cmp_f fcmp) {
    if (h == NULL || h->len <= 0)
        return NULL;

    // 开始查找这个节点
    unsigned i = 0;
    fcmp = fcmp ? fcmp : h->fcmp;
    do {
        void * node = h->data[i];
        if (fcmp(arg, node) == 0) {
            // 找到节点开始走删除操作
            if (--h->len > 0 && h->len != i) {
                // 尾巴节点和待删除节点比较
                int ret = h->fcmp(h->data[h->len], node);

                // 小顶堆, 新的值比老的值小, 那么上浮
                if (ret < 0)
                    up(h->fcmp, h->data[h->len], h->data, i);
                else if (ret > 0) {
                    // 小顶堆, 新的值比老的值大, 那么下沉
                    h->data[i] = h->data[h->len];
                    down(h->fcmp, h->data, h->len, i);
                }
            }

            return node;
        }
    } while (++i < h->len);

    return NULL;
}

//
// heap_top - 查看堆顶节点数据
// h        : 堆对象
// return   : 堆顶节点
//
inline void * 
heap_top(heap_t h) {
    return h->len <= 0 ? NULL : *h->data;
}

//
// heap_top - 摘掉堆顶节点数据
// h        : 堆对象
// return   : 返回堆顶节点
//
inline void * 
heap_pop(heap_t h) {
    void * node = heap_top(h);
    if (node && --h->len > 0) {
        // 尾巴节点一定比小堆顶节点大, 那么要下沉
        h->data[0] = h->data[h->len];
        down(h->fcmp, h->data, h->len, 0);
    }
    return node;
}
