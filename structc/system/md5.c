#include "md5.h"

struct md5c {
    uint8_t  in[64];      /* Input data */
    uint32_t nl, nh;      /* Number of _bits_ handled mod 2^64 */
    uint32_t a, b, c, d;  /* Scratch buffer a, b, c, d */
};

/* MD5_F, MD5_G and MD5_H are basic MD5 functions: selection, majority, parity */
#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE rotates x left n bits */
#define ROTATE(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* MD5_FF, MD5_GG, MD5_HH, and MD5_II transformations for */
/* rounds 1, 2, 3, and 4. Rotation is separate from addition to prevent recomputation */
#define MD5_FF(a, b, c, d, x, s, ac) {                   \
    (a) += MD5_F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE ((a), (s)); (a) += (b);                 \
}

#define MD5_GG(a, b, c, d, x, s, ac) {                   \
    (a) += MD5_G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE ((a), (s)); (a) += (b);                 \
}

#define MD5_HH(a, b, c, d, x, s, ac) {                   \
    (a) += MD5_H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE ((a), (s)); (a) += (b);                 \
}

#define MD5_II(a, b, c, d, x, s, ac) {                   \
    (a) += MD5_I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE ((a), (s)); (a) += (b);                 \
}

/* Constants for transformation */
#define MD5_S11  7 /* Round 1 */
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21  5 /* Round 2 */
#define MD5_S22  9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31  4 /* Round 3 */
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41  6 /* Round 4 */
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

/* Basic MD5 step. MD5_Transform buf based on in */
extern void md5_transform(struct md5c * ctx, const uint32_t * in) {
    uint32_t a = ctx->a, b = ctx->b, c = ctx->c, d = ctx->d;

    /* Round 1 */
    MD5_FF ( a, b, c, d, in[ 0], MD5_S11, 3614090360u); /*  1 */
    MD5_FF ( d, a, b, c, in[ 1], MD5_S12, 3905402710u); /*  2 */
    MD5_FF ( c, d, a, b, in[ 2], MD5_S13,  606105819u); /*  3 */
    MD5_FF ( b, c, d, a, in[ 3], MD5_S14, 3250441966u); /*  4 */
    MD5_FF ( a, b, c, d, in[ 4], MD5_S11, 4118548399u); /*  5 */
    MD5_FF ( d, a, b, c, in[ 5], MD5_S12, 1200080426u); /*  6 */
    MD5_FF ( c, d, a, b, in[ 6], MD5_S13, 2821735955u); /*  7 */
    MD5_FF ( b, c, d, a, in[ 7], MD5_S14, 4249261313u); /*  8 */
    MD5_FF ( a, b, c, d, in[ 8], MD5_S11, 1770035416u); /*  9 */
    MD5_FF ( d, a, b, c, in[ 9], MD5_S12, 2336552879u); /* 10 */
    MD5_FF ( c, d, a, b, in[10], MD5_S13, 4294925233u); /* 11 */
    MD5_FF ( b, c, d, a, in[11], MD5_S14, 2304563134u); /* 12 */
    MD5_FF ( a, b, c, d, in[12], MD5_S11, 1804603682u); /* 13 */
    MD5_FF ( d, a, b, c, in[13], MD5_S12, 4254626195u); /* 14 */
    MD5_FF ( c, d, a, b, in[14], MD5_S13, 2792965006u); /* 15 */
    MD5_FF ( b, c, d, a, in[15], MD5_S14, 1236535329u); /* 16 */

    /* Round 2 */
    MD5_GG ( a, b, c, d, in[ 1], MD5_S21, 4129170786u); /* 17 */
    MD5_GG ( d, a, b, c, in[ 6], MD5_S22, 3225465664u); /* 18 */
    MD5_GG ( c, d, a, b, in[11], MD5_S23,  643717713u); /* 19 */
    MD5_GG ( b, c, d, a, in[ 0], MD5_S24, 3921069994u); /* 20 */
    MD5_GG ( a, b, c, d, in[ 5], MD5_S21, 3593408605u); /* 21 */
    MD5_GG ( d, a, b, c, in[10], MD5_S22,   38016083u); /* 22 */
    MD5_GG ( c, d, a, b, in[15], MD5_S23, 3634488961u); /* 23 */
    MD5_GG ( b, c, d, a, in[ 4], MD5_S24, 3889429448u); /* 24 */
    MD5_GG ( a, b, c, d, in[ 9], MD5_S21,  568446438u); /* 25 */
    MD5_GG ( d, a, b, c, in[14], MD5_S22, 3275163606u); /* 26 */
    MD5_GG ( c, d, a, b, in[ 3], MD5_S23, 4107603335u); /* 27 */
    MD5_GG ( b, c, d, a, in[ 8], MD5_S24, 1163531501u); /* 28 */
    MD5_GG ( a, b, c, d, in[13], MD5_S21, 2850285829u); /* 29 */
    MD5_GG ( d, a, b, c, in[ 2], MD5_S22, 4243563512u); /* 30 */
    MD5_GG ( c, d, a, b, in[ 7], MD5_S23, 1735328473u); /* 31 */
    MD5_GG ( b, c, d, a, in[12], MD5_S24, 2368359562u); /* 32 */

    /* Round 3 */
    MD5_HH ( a, b, c, d, in[ 5], MD5_S31, 4294588738u); /* 33 */
    MD5_HH ( d, a, b, c, in[ 8], MD5_S32, 2272392833u); /* 34 */
    MD5_HH ( c, d, a, b, in[11], MD5_S33, 1839030562u); /* 35 */
    MD5_HH ( b, c, d, a, in[14], MD5_S34, 4259657740u); /* 36 */
    MD5_HH ( a, b, c, d, in[ 1], MD5_S31, 2763975236u); /* 37 */
    MD5_HH ( d, a, b, c, in[ 4], MD5_S32, 1272893353u); /* 38 */
    MD5_HH ( c, d, a, b, in[ 7], MD5_S33, 4139469664u); /* 39 */
    MD5_HH ( b, c, d, a, in[10], MD5_S34, 3200236656u); /* 40 */
    MD5_HH ( a, b, c, d, in[13], MD5_S31,  681279174u); /* 41 */
    MD5_HH ( d, a, b, c, in[ 0], MD5_S32, 3936430074u); /* 42 */
    MD5_HH ( c, d, a, b, in[ 3], MD5_S33, 3572445317u); /* 43 */
    MD5_HH ( b, c, d, a, in[ 6], MD5_S34,   76029189u); /* 44 */
    MD5_HH ( a, b, c, d, in[ 9], MD5_S31, 3654602809u); /* 45 */
    MD5_HH ( d, a, b, c, in[12], MD5_S32, 3873151461u); /* 46 */
    MD5_HH ( c, d, a, b, in[15], MD5_S33,  530742520u); /* 47 */
    MD5_HH ( b, c, d, a, in[ 2], MD5_S34, 3299628645u); /* 48 */

    /* Round 4 */
    MD5_II ( a, b, c, d, in[ 0], MD5_S41, 4096336452u); /* 49 */
    MD5_II ( d, a, b, c, in[ 7], MD5_S42, 1126891415u); /* 50 */
    MD5_II ( c, d, a, b, in[14], MD5_S43, 2878612391u); /* 51 */
    MD5_II ( b, c, d, a, in[ 5], MD5_S44, 4237533241u); /* 52 */
    MD5_II ( a, b, c, d, in[12], MD5_S41, 1700485571u); /* 53 */
    MD5_II ( d, a, b, c, in[ 3], MD5_S42, 2399980690u); /* 54 */
    MD5_II ( c, d, a, b, in[10], MD5_S43, 4293915773u); /* 55 */
    MD5_II ( b, c, d, a, in[ 1], MD5_S44, 2240044497u); /* 56 */
    MD5_II ( a, b, c, d, in[ 8], MD5_S41, 1873313359u); /* 57 */
    MD5_II ( d, a, b, c, in[15], MD5_S42, 4264355552u); /* 58 */
    MD5_II ( c, d, a, b, in[ 6], MD5_S43, 2734768916u); /* 59 */
    MD5_II ( b, c, d, a, in[13], MD5_S44, 1309151649u); /* 60 */
    MD5_II ( a, b, c, d, in[ 4], MD5_S41, 4149444226u); /* 61 */
    MD5_II ( d, a, b, c, in[11], MD5_S42, 3174756917u); /* 62 */
    MD5_II ( c, d, a, b, in[ 2], MD5_S43,  718787259u); /* 63 */
    MD5_II ( b, c, d, a, in[ 9], MD5_S44, 3951481745u); /* 64 */

    ctx->a += a; ctx->b += b; ctx->c += c; ctx->d += d;
}

// Set pseudoRandomNumber to zero for RFC MD5 implementation
extern inline void md5_init(struct md5c * ctx) {
    ctx->nl = ctx->nh = 0;

    /* Load magic initialization constants */
    ctx->a = 0x67452301;
    ctx->b = 0xefcdab89;
    ctx->c = 0x98badcfe;
    ctx->d = 0x10325476;
}

extern void md5_update(struct md5c * ctx, const void * buf, size_t n) {
    uint32_t in[16];
    uint32_t i, ii, mdi;
    uint32_t len = (uint32_t)n;
    const uint8_t * input = buf;

    /* Compute number of bytes mod 64 */
    mdi =  (ctx->nl >> 3) & 0x3F;

    /* Update number of bits */
    if (ctx->nl + (len << 3) < ctx->nl)
        ++ ctx->nh;
    ctx->nl += len <<  3;
    ctx->nh += len >> 29;

    while (len--) {
        /* Add new character to buffer, increment mdi */
        ctx->in[mdi++] = *input++;

        /* Transform if necessary */
        if (mdi == 0x40) {
            for (i = ii = 0; i < 16; ++i, ii += 4)
                in[i] = (((uint32_t)ctx->in[ii + 3]) << 3 * 8) |
                        (((uint32_t)ctx->in[ii + 2]) << 2 * 8) |
                        (((uint32_t)ctx->in[ii + 1]) << 1 * 8) |
                        (((uint32_t)ctx->in[ii + 0]) << 0 * 8) ;

            md5_transform (ctx, in);
            mdi = 0;
        }
    }
}

extern void md5_final(struct md5c * ctx, uint8_t digest[16]) {
    uint32_t in[16];
    uint32_t mdi, i, ii, padn;
    static uint8_t padding[64] = { 0x80 };

    /* Save number of bits */
    in[14] = ctx->nl;
    in[15] = ctx->nh;

    /* Compute number of bytes mod 64 */
    mdi = (ctx->nl >> 3) & 0x3F;

    /* Pad out to 56 mod 64 */
    padn = mdi < 56 ? 56 - mdi : 120 - mdi;
    md5_update (ctx, padding, padn);

    /* Append length in bits and transform */
    for (i = ii = 0; i < 14; ++i, ii += 4)
        in[i] = (((uint32_t)ctx->in[ii + 3]) << 3 * 8) |
                (((uint32_t)ctx->in[ii + 2]) << 2 * 8) |
                (((uint32_t)ctx->in[ii + 1]) << 1 * 8) |
                (((uint32_t)ctx->in[ii + 0]) << 0 * 8) ;
    md5_transform (ctx, in);

    /* Store buffer in digest */
    digest[4 * 0 + 0] = (uint8_t)((ctx->a >> 0 * 8) & 0xFF);
    digest[4 * 0 + 1] = (uint8_t)((ctx->a >> 1 * 8) & 0xFF);
    digest[4 * 0 + 2] = (uint8_t)((ctx->a >> 2 * 8) & 0xFF);
    digest[4 * 0 + 3] = (uint8_t)((ctx->a >> 3 * 8) & 0xFF);
                                              
    digest[4 * 1 + 0] = (uint8_t)((ctx->b >> 0 * 8) & 0xFF);
    digest[4 * 1 + 1] = (uint8_t)((ctx->b >> 1 * 8) & 0xFF);
    digest[4 * 1 + 2] = (uint8_t)((ctx->b >> 2 * 8) & 0xFF);
    digest[4 * 1 + 3] = (uint8_t)((ctx->b >> 3 * 8) & 0xFF);

    digest[4 * 2 + 0] = (uint8_t)((ctx->c >> 0 * 8) & 0xFF);
    digest[4 * 2 + 1] = (uint8_t)((ctx->c >> 1 * 8) & 0xFF);
    digest[4 * 2 + 2] = (uint8_t)((ctx->c >> 2 * 8) & 0xFF);
    digest[4 * 2 + 3] = (uint8_t)((ctx->c >> 3 * 8) & 0xFF);

    digest[4 * 3 + 0] = (uint8_t)((ctx->d >> 0 * 8) & 0xFF);
    digest[4 * 3 + 1] = (uint8_t)((ctx->d >> 1 * 8) & 0xFF);
    digest[4 * 3 + 2] = (uint8_t)((ctx->d >> 2 * 8) & 0xFF);
    digest[4 * 3 + 3] = (uint8_t)((ctx->d >> 3 * 8) & 0xFF);
}

// md5_convert 将16位 md5 转成 32 位 md5码
uint8_t * md5_convert(md5_t m, const uint8_t digest[16]) {
    uint8_t i = 0, * o = m;
    while (i < 16) {
        uint8_t x = digest[i++];
        // 潜规则走小写 MD5
        *o++ = (uint8_t)"0123456789abcdefg"[x >> 4];
        *o++ = (uint8_t)"0123456789abcdefg"[x & 15];
    }
    *o = '\0';
    return m;
}

//
// md5_strs - 得到字符串的 md5 串
// m        : 存储 md5串地址
// d        : 内存块数据
// n        : 内存块长度
// return   : m 首地址
//
inline uint8_t * 
md5_data(md5_t m, const void * d, size_t n) {
    struct md5c ctx;
    uint8_t digest[16];

    md5_init(&ctx);
    md5_update(&ctx, d, n);
    md5_final(&ctx, digest);

    return md5_convert(m, digest);
}

//
// md5_strs - 得到字符串的 md5 串
// m        : 存储 md5串地址
// p        : 文件的路径
// return   : m 首地址
//
uint8_t * 
md5_file(md5_t m, const char * p) {
    size_t n;
    FILE * txt;
    struct md5c ctx;
    uint8_t digest[16], d[BUFSIZ];
    if ((txt = fopen(p, "rb")) == NULL) 
        return m;

    md5_init(&ctx);

    do {
        // 读取数据
        n = fread(d, sizeof(uint8_t), sizeof d, txt);
        if (ferror(txt)) {
            fclose(txt);
            return m;
        }
        md5_update(&ctx, d, n);
    } while (n == sizeof d);
    fclose(txt);

    md5_final(&ctx, digest);

    return md5_convert(m, digest);
}
