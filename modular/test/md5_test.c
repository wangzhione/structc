#include <md5.h>

//
// md5 (12345678, 32) = 25d55ad283aa400af464c76d713c07ad
// md5 (12345678, 16) = 83aa400af464c76d [忽略]
//
void md5_test(void) {
    const char * key = "12345678";

    md5_t m;
    printf("md5 (%s, 32) => %s\n", key, md5_str(m, key));
}
