#include <md5.h>
#include <sha1.h>

//
// md5 (12345678, 32) = 25d55ad283aa400af464c76d713c07ad
// md5 (12345678, 16) = 83aa400af464c76d [忽略]
// sha1(12345678, 40) = 7c222fb2927d828af22f592134e8932480637c0d
//
void md5_test(void) {
    const char * key = "12345678";

    md5_t m;
    printf("md5 (%s, 32) => %s\n", key, md5_str(m, key));

    sha1_t a;
    printf("sha1(%s, 40) = %s\n", key, sha1_str(a, key));
}
