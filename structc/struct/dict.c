#include "dict.h"

//
// primes - 质数表
//
const unsigned primes[][2] = {
    { (1<<6)-1  ,         53 },
    { (1<<7)-1  ,         97 },
    { (1<<8)-1  ,        193 },
    { (1<<9)-1  ,        389 },
    { (1<<10)-1 ,        769 },
    { (1<<11)-1 ,       1543 },
    { (1<<12)-1 ,       3079 },
    { (1<<13)-1 ,       6151 },
    { (1<<14)-1 ,      12289 },
    { (1<<15)-1 ,      24593 },
    { (1<<16)-1 ,      49157 },
    { (1<<17)-1 ,      98317 },
    { (1<<18)-1 ,     196613 },
    { (1<<19)-1 ,     393241 },
    { (1<<20)-1 ,     786433 },
    { (1<<21)-1 ,    1572869 },
    { (1<<22)-1 ,    3145739 },
    { (1<<23)-1 ,    6291469 },
    { (1<<24)-1 ,   12582917 },
    { (1<<25)-1 ,   25165843 },
    { (1<<26)-1 ,   50331653 },
    { (1<<27)-1 ,  100663319 },
    { (1<<28)-1 ,  201326611 },
    { (1<<29)-1 ,  402653189 },
    { (1<<30)-1 ,  805306457 },
    { UINT_MAX  , 1610612741 },
};

struct keypair {
    struct keypair * next;
    unsigned hash;
    void * val;
    char key[];
};

// keypair_delete - 销毁节点数据
inline void keypair_delete(node_f fdie, struct keypair * pair) {
    if (pair->val && fdie)
        fdie(pair->val);
    free(pair);
}

// keypair_create - 创建节点数据
inline struct keypair * keypair_create(unsigned hash, void * v, const char * k) {
    size_t len = strlen(k) + 1;
    struct keypair * pair = malloc(sizeof(struct keypair) + len);
    pair->hash = hash;
    pair->val = v;
    memcpy(pair->key, k, len);
    return pair;
}

struct dict {
    node_f fdie;                // 节点注册的销毁函数
    unsigned idx;               // 使用 _primes 质数表索引
    unsigned used;              // 用户已经使用的节点个数
    struct keypair ** table;    // size = primes[idx][0]
};

static void dict_resize(struct dict * d) {
    unsigned size, prime, i;
    struct keypair ** table;
    unsigned used = d->used;

    if (used < primes[d->idx][0])
        return;
    
    // 构造新的内存布局大小
    size = primes[d->idx][1];
    prime = primes[++d->idx][1];
    table = calloc(prime, sizeof(struct keypair *));

    // 开始转移数据
    for (i = 0; i < size; ++i) {
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
    unsigned i, size;
    if (NULL == d) return;
    size = primes[d->idx][1];
    for (i = 0; i < size; ++i) {
        struct keypair * pair = d->table[i];
        while (pair) {
            struct keypair * next = pair->next;
            keypair_delete(d->fdie, pair);
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
dict_create(node_f fdie) {
    struct dict * d = malloc(sizeof(struct dict));
    unsigned size = primes[d->idx = 0][1];
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
    assert(NULL != d && k != NULL);

    hash = str_hash(k);
    index = hash % primes[d->idx][1];
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
    assert(NULL != d && k != NULL);

    // 检查一下内存, 看是否要扩充
    dict_resize(d);

    // 开始寻找数据
    hash = str_hash(k);
    index = hash % primes[d->idx][1];
    pair = d->table[index];
    prev = NULL;

    while (pair) {
        // 找见了数据
        if (pair->hash == hash && !strcmp(pair->key, k)) {
            // 相同数据直接返回什么都不操作
            if (pair->val == v)
                return;

            // 删除操作
            if (NULL == v) {
                if (NULL == prev)
                    d->table[index] = pair->next;
                else
                    prev->next = pair->next;

                // 销毁节点, 直接返回
                return keypair_delete(d->fdie, pair);
            }

            // 更新节点
            if (d->fdie)
                d->fdie(pair->val);
            pair->val = v;
            return;
        }

        prev = pair;
        pair = pair->next;
    }

    // 没有找见设置操作, 直接插入数据
    if (NULL != v) {
        pair = keypair_create(hash, v, k);
        pair->next = d->table[index];
        d->table[index] = pair;
        ++d->used;
    }
}
