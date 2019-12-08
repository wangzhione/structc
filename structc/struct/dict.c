#include "dict$table.h"

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
    unsigned idx;               // 使用 primes 质数表索引
    unsigned used;              // 用户已经使用的结点个数
    struct keypair ** table;    // size = primes[idx][0]
};

static void dict_resize(struct dict * d) {
    unsigned size, prime, i;
    struct keypair ** table;
    unsigned used = d->used;

    if (used < prime_table[d->idx][0])
        return;
    
    // 构造新的内存布局大小
    size = prime_table[d->idx][1];
    prime = prime_table[++d->idx][1];
    table = calloc(prime, sizeof(struct keypair *));

    // 开始转移数据
    for (i = 0; i < size; i++) {
        struct keypair * pair = d->table[i];
        while (pair) {
            struct keypair * next = pair->next;
            unsigned index = pair->hash % prime;

            pair->next = table[index];
            table[index] = pair;
            pair = next;
        }
    }

    // table 重新变化
    free(d->table);
    d->table = table;
}

//
// dict_delete - 字典删除
// d        : dict_create 创建的字典对象
// return   : void 
//
void 
dict_delete(dict_t d) {
    if (!d) return;

    unsigned size = prime_table[d->idx][1];
    for (unsigned i = 0; i < size; i++) {
        struct keypair * pair = d->table[i];
        while (pair) {
            struct keypair * next = pair->next;
            keypair_delete(pair, d->fdie);
            pair = next;
        }
    }
    free(d->table);
    free(d);
}

//
// dict_create - 字典创建
// fdie     : v 销毁函数
// return   : dict_t
//
inline dict_t 
dict_create(void * fdie) {
    struct dict * d = malloc(sizeof(struct dict));
    unsigned size = prime_table[d->idx = 0][1];
    d->used = 0;
    d->fdie = fdie;
    // 默认构建的第一个素数表 index = 0
    d->table = calloc(size, sizeof(struct keypair *));
    return d;
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

    hash = str_hash(k);
    index = hash % prime_table[d->idx][1];
    pair = d->table[index];

    while (pair) {
        if (!strcmp(pair->key, k))
            return pair->val;
        pair = pair->next;
    }

    return NULL;
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
    unsigned hash, index;
    struct keypair * pair, * prev;
    assert(d && k);

    // 检查一下内存, 看是否要扩充
    dict_resize(d);

    // 开始寻找数据
    hash = str_hash(k);
    index = hash % prime_table[d->idx][1];
    pair = d->table[index];
    prev = NULL;

    while (pair) {
        // 找见了数据
        if (pair->hash == hash && !strcmp(pair->key, k)) {
            // 相同数据直接返回什么都不操作
            if (pair->val == v)
                return;

            // 删除操作
            if (!v) {
                if (!prev)
                    d->table[index] = pair->next;
                else
                    prev->next = pair->next;

                // 销毁结点, 直接返回
                return keypair_delete(pair, d->fdie);
            }

            // 更新结点
            if (d->fdie)
                d->fdie(pair->val);
            pair->val = v;
            return;
        }

        prev = pair;
        pair = pair->next;
    }

    // 没有找见设置操作, 直接插入数据
    if (v) {
        pair = keypair_create(hash, v, k);
        pair->next = d->table[index];
        d->table[index] = pair;
        ++d->used;
    }
}
