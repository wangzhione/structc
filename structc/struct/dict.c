#include <dict.h>

//
// 质数表
//
static const unsigned primes[][2] = {
    { (2^6 )-1,         53 },
    { (2^7 )-1,         97 },
    { (2^8 )-1,        193 },
    { (2^9 )-1,        389 },
    { (2^10)-1,        769 },
    { (2^11)-1,       1543 },
    { (2^12)-1,       3079 },
    { (2^13)-1,       6151 },
    { (2^14)-1,      12289 },
    { (2^15)-1,      24593 },
    { (2^16)-1,      49157 },
    { (2^17)-1,      98317 },
    { (2^18)-1,     196613 },
    { (2^19)-1,     393241 },
    { (2^20)-1,     786433 },
    { (2^21)-1,    1572869 },
    { (2^22)-1,    3145739 },
    { (2^23)-1,    6291469 },
    { (2^24)-1,   12582917 },
    { (2^25)-1,   25165843 },
    { (2^26)-1,   50331653 },
    { (2^27)-1,  100663319 },
    { (2^28)-1,  201326611 },
    { (2^29)-1,  402653189 },
    { (2^30)-1,  805306457 },
    { UINT_MAX, 1610612741 },
};

struct keypair {
    struct keypair * next;
    unsigned hash;
    void * val;
    char key[];
};

struct dict {
    node_f fdie;                // 结点注册的销毁函数
    unsigned idx;               // 使用的质数表索引
    unsigned used;              // 用户已经使用的结点个数
    struct keypair ** table;    // size = primes[idx][0]
};

// 销毁结点数据
static inline void _keypair_delete(struct dict * d, struct keypair * pair) {
    if (pair->val && d->fdie)
        d->fdie(pair->val);
    free(pair);
}

// 创建结点数据
static inline struct keypair * _keypair_create(unsigned hash, void * v, const char * k) {
    size_t len = strlen(k) + 1;
    struct keypair * pair = malloc(sizeof(struct keypair) + len);
    pair->hash = hash;
    pair->val = v;
    memcpy(pair->key, k, len);
    return pair;
}

static void _dict_resize(struct dict * d) {
    unsigned size, prime, i;
    struct keypair ** table;
    unsigned used = d->used;

    if (used < primes[d->idx][0])
        return;
    
    // 构造新的内存布局大小
    ++d->idx;
    size = primes[d->idx][0];
    prime = primes[d->idx][1];
    table = calloc(size, sizeof(struct keypair *));

    // 开始转移数据
    for (i = 0; i < used; ++i) {
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
