#include "sha1.h"

// rol 宏 让 val 循环左移 bits
#define rol(val, bits) (((val) << (bits)) | ((val) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
#ifdef ISBIG
#  define blk0(i)  block->l[i]
#else
#  define blk0(i) (block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00)\
                               | (rol(block->l[i],  8) & 0x00FF00FF))
#endif

#define blk(i) (block->l[15&(i)] = rol(block->l[15&(i+13)]          \
                                     ^ block->l[15&(i+8) ]          \
                                     ^ block->l[15&(i+2) ]          \
                                     ^ block->l[15&(i)   ]          \
                                 , 1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v, w, x, y, z, i)                                        \
do {                                                                \
    z += ((w&(x^y))^y) + blk0(i) + 0x5A827999 + rol(v, 5);          \
    w = rol(w, 30);                                                 \
} while(0)
#define R1(v, w, x, y, z, i)                                        \
do {                                                                \
    z += ((w&(x^y))^y) + blk(i) + 0x5A827999 + rol(v, 5);           \
    w = rol(w, 30);                                                 \
} while(0)
#define R2(v, w, x, y, z, i)                                        \
do {                                                                \
    z += (w^x^y) + blk(i) + 0x6ED9EBA1 + rol(v, 5);                 \
    w = rol(w, 30);                                                 \
} while(0)
#define R3(v, w, x, y, z, i)                                        \
do {                                                                \
    z += (((w|x)&y)|(w&x)) + blk(i) + 0x8F1BBCDC + rol(v, 5);       \
    w = rol(w, 30);                                                 \
} while(0)
#define R4(v, w, x, y, z, i)                                        \
do {                                                                \
    z += (w^x^y) + blk(i) + 0xCA62C1D6 + rol(v, 5);                 \
    w = rol(w, 30);                                                 \
} while(0) 

/* Hash a single 512-bit block. This is the core of the algorithm. */
extern void sha1_transform(uint32_t state[5], const uint8_t buffer[64]) {
    /* Copy context->state[] to working vars */
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];

    /* use array to appear as a pointer copy buffer memory */
    union {
        uint8_t  c[64]; 
        uint32_t l[16];
    } block[1];
    memcpy(block, buffer, sizeof block);

    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
    
    /* Add the working vars back into context.state[] */
    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
}

// sha1_init - Initialize new context
extern void sha1_init(struct sha1 * ctx) {
    /* sha1 initialization constants */
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count[0] = ctx->count[1] = 0;
}

/* Run your data through this. */
extern void sha1_update(struct sha1 * ctx, const uint8_t * data, uint32_t len) {
    // 设置 count bits 数
    uint32_t i, j = ctx->count[0];
    if ((ctx->count[0] += len << 3) < j)
        ctx->count[1]++;
    ctx->count[1] += len >> 29;

    // 获取低 6 位 byte 值
    j = (j >> 3) & 63;
    if ((j + len) <= 63)
        i = 0;
    else {
        memcpy(ctx->buffer + j, data, (i = 64 - j));
        sha1_transform(ctx->state, ctx->buffer);
        for ( ; i + 63 < len; i += 64)
            sha1_transform(ctx->state, data + i);
        j = 0;
    }
    memcpy(ctx->buffer + j, data + i, len - i);
}

/* Add padding and return the message digest. */
extern void sha1_final(struct sha1 * ctx, uint8_t digest[20]) {
    unsigned i;
    uint8_t c, finalcount[8];

    for (i = 0; i < 8; i++) {
        /* Endian independent */
        finalcount[i] = (uint8_t)((ctx->count[(i >= 4 ? 0 : 1)] >> ((3-(i & 3)) * 8) ) & 255);
    }

    c = 0200;
    sha1_update(ctx, &c, 1);
    while ((ctx->count[0] & 504) != 448) {
	    c = 0000;
        sha1_update(ctx, &c, 1);
    }
    sha1_update(ctx, finalcount, 8);  /* Should cause a SHA1Transform() */
    for (i = 0; i < 20; i++) {
        digest[i] = (uint8_t)((ctx->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
}

// sha1_convert 将 20 byte sha1 转成 41 byte sha1 C 字符串码
static uint8_t * sha1_convert(sha1_t a, const uint8_t digest[20]) {
    uint8_t * o = a, i = 0;
    while (i < 20) {
        uint8_t x = digest[i++];
        // 默认走小写编码
        *o++ = "0123456789abcdef"[x >> 4];
        *o++ = "0123456789abcdef"[x & 15];
    }
    *o = '\0';
    return a;
}

uint8_t * 
sha1_file(sha1_t a, const char * path) {
    struct sha1 ctx;
    uint8_t digest[20], data[BUFSIZ];
    FILE * stream = fopen(path, "rb");
    if (!stream) return a;

    sha1_init(&ctx);
    do {
        // 读取数据
        size_t n = fread(data, sizeof(uint8_t), sizeof data, stream);
        if (ferror(stream)) {
            fclose(stream);
            return a;
        }
        sha1_update(&ctx, data, (uint32_t)n);
    } while (!feof(stream));
    fclose(stream);
    sha1_final(&ctx, digest);

    return sha1_convert(a, digest);    
}

uint8_t * 
sha1_data(sha1_t a, const void * data, size_t n) {
    struct sha1 ctx;
    uint8_t digest[20];

    sha1_init(&ctx);
    sha1_update(&ctx, data, (uint32_t)n);
    sha1_final(&ctx, digest);

    return sha1_convert(a, digest);    
}
