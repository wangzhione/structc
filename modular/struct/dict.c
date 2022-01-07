#include "dict.h"

#define DICT_INIT_UINT (1u<<6)

struct keypair {
    struct keypair * next;
    unsigned hash;
    void * val;
    char key[];
};

// keypair_delete - 销毁结点数据
inline void keypair_delete(struct keypair * pair, node_f fdie) {
    if (pair->val && fdie)
        fdie(pair->val);
    free(pair);
}

// keypair_create - 创建结点数据
inline struct keypair * keypair_create(unsigned hash, 
                                       void * v, const char * k) {
    size_t len = strlen(k) + 1;
    struct keypair * pair = malloc(sizeof(struct keypair) + len);
    pair->hash = hash;
    pair->val = v;
    memcpy(pair->key, k, len);
    return pair;
}

struct dict {
    node_f fdie;                // 结点注册的销毁函数
    unsigned used;              // 用户已经使用的结点个数
    unsigned size;              // 结点容量
    struct keypair ** table;    // 集合
};

static void dict_resize(struct dict * d) {
    struct keypair ** table;
    unsigned used = d->used;
    unsigned size = d->size;

    // 数据容量变化 按照简单倍率扩张和收缩
    if (used >= size) {
        size <<= 1;
    } else {
        // 拍脑门算法
        if (used >= size >> 2 || size <= DICT_INIT_UINT)
            return;
        size >>= 1;
    }
    
    // 构造新的内存布局大小
    table = calloc(size, sizeof(struct keypair *));

    // 开始转移数据
    for (unsigned i = 0; i < d->size; i++) {
        struct keypair * pair = d->table[i];
        while (pair) {
            struct keypair * next = pair->next;
            // 取余
            unsigned index = pair->hash & (size - 1);

            pair->next = table[index];
            table[index] = pair;
            pair = next;
        }
    }

    // table 重新变化
    free(d->table);
    d->table = table;
    d->size = size;
}

void dict_delete_partial(dict_t d) {
    free(d->table);
    free(d);
}

//
// dict_delete - 字典删除
// d        : dict_create 创建的字典对象
// return   : void 
//
void 
dict_delete(dict_t d) {
    if (!d) return;

    for (unsigned i = 0; i < d->size; i++) {
        struct keypair * pair = d->table[i];
        while (pair) {
            struct keypair * next = pair->next;
            keypair_delete(pair, d->fdie);
            pair = next;
        }
    }

    dict_delete_partial(d);
}

//
// dict_create - 字典创建
// fdie     : v 销毁函数
// return   : dict_t
//
inline dict_t 
dict_create(void * fdie) {
    struct dict * d = malloc(sizeof(struct dict));
    d->used = 0;
    d->size = DICT_INIT_UINT;
    d->fdie = fdie;
    // 默认构建的第一个素数表 index = 0
    d->table = calloc(DICT_INIT_UINT, sizeof(struct keypair *));
    return d;
}

static unsigned SDBMHash(const char * k) {
    register unsigned o,h = 0u;
    while ((o = *k++))
        h = o + h * 65599u;
    return h;
}

//
// dict_get - 获取字典中对映的 v
// d        : dict_create 创建的字典对象
// k        : 查找的 key 
// return   : 查找的 v, NULL 表示没有
//
void * 
dict_get(dict_t d, const char * k) {
    unsigned hash, index;
    struct keypair * pair;
    assert(d && k);

    hash = SDBMHash(k);
    index = hash & (d->size - 1);
    pair = d->table[index];

    while (pair) {
        if (!strcmp(pair->key, k))
            return pair->val;
        pair = pair->next;
    }

    return NULL;
}

void dict_del(dict_t d, const char * k) {
    unsigned hash = SDBMHash(k);
    unsigned index = hash & (d->size - 1);
    struct keypair * pair = d->table[index];
    struct keypair * prev = NULL;

    while (pair) {
        // 找见了数据
        if (pair->hash == hash && !strcmp(pair->key, k)) {
            // 删除操作
            if (NULL == prev)
                d->table[index] = pair->next;
            else
                prev->next = pair->next;

            // 销毁结点并尝试缩减容量
            keypair_delete(pair, d->fdie);
            --d->used;
            return dict_resize(d);
        }

        prev = pair;
        pair = pair->next;
    }
}

//
// dict_set - 设置一个 <k, v> 结构
// d        : dict_create 创建的字典对象
// k        : 插入的 key
// v        : 插入数据的值, NULL 会销毁 k
// return   : void
//
void 
dict_set(dict_t d, const char * k, void * v) {
    assert(d && k);

    // 走删除分支
    if (NULL == v) {
        return dict_del(d, k);
    }

    // 检查一下内存, 看是否要扩充
    dict_resize(d);

    // 开始寻找数据
    unsigned hash = SDBMHash(k);
    unsigned index = hash & (d->size - 1);
    struct keypair * pair = d->table[index];

    while (pair) {
        // 找见了数据
        if (pair->hash == hash && !strcmp(pair->key, k)) {
            // 相同数据直接返回什么都不操作
            if (pair->val == v)
                return;

            // 更新结点
            if (d->fdie)
                d->fdie(pair->val);
            pair->val = v;
            return;
        }

        pair = pair->next;
    }

    // 没有找见设置操作, 直接插入数据
    pair = keypair_create(hash, v, k);
    pair->next = d->table[index];
    d->table[index] = pair;
    ++d->used;
}

void dict_add_keypair(dict_t d, struct keypair * prev) {
    // 先检查内存是否够用
    dict_resize(d);

    const char * k = prev->key;
    unsigned hash = prev->hash;
    unsigned index = hash & (d->size - 1);

    void * val = prev->val;
    struct keypair * pair = d->table[index];

    while (pair) {
        // 相同内存, 什么都不用处理
        if (pair == prev) {
            POUT("incorrect usage 1 pair=%p, key=%s", pair, pair->key);
            return;
        }

        // 找见了数据
        if (pair->hash == hash && !strcmp(pair->key, k)) {
            // 相同数据, 不推荐, 最好追查下
            if (pair->val == val) {
                POUT("incorrect usage 2 pair=%p, key=%s, prev = %p", pair, pair->key, prev);
                return keypair_delete(prev, NULL);
            }

            // 交互结点, 删除之前结点
            prev->val = pair->val;
            pair->val = val;
            return keypair_delete(prev, d->fdie);
        }

        pair = pair->next;
    }

    prev->next = d->table[index];
    d->table[index] = prev;
    ++d->used;
} 

// d += a ; delete a; return d; 
void dict_add_delete_partial(dict_t d, dict_t a) {
    unsigned index;
    struct keypair * pair, * prev;

    for (index = 0; index < a->size && a->used > 0; ++index) {
        pair = a->table[index];

        while (pair) {
            // a 结点 move d 上
            prev = pair;
            pair = pair->next;
            dict_add_keypair(d, prev);
            --a->used;
        }
    }

    dict_delete_partial(a);
}

void dict_add_delete(dict_t * pd, dict_t * pa) {
    assert(pd && pa && (*pd)->fdie == (*pa)->fdie);

    dict_t d = *pd, a = *pa;

    // step 1 : a is NULL, 什么都不需要操作
    if (NULL == a) {
        return;
    }

    // pa 提前清理
    *pa = NULL;

    // step 1 : d is NULL, a -> d
    if (NULL == d) {
        *pd = a;
        return;
    }

    // step 3 : a not is NULL and d not is NULL
    // step 3.1 : a 更适合, swap a, b
    if (d->size < a->size && d->used < a->used) {
        dict_t temp = d;
        *pd = d = a;
        a = temp;
    }
    dict_add_delete_partial(d, a);
}
