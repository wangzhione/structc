#include <md5.h>
//
// md5(12345678,32) = 25d55ad283aa400af464c76d713c07ad
// md5(12345678,16) = 83aa400af464c76d
//
void md5_test(void) {
    md5_t m;
    const char * key = "12345678";
    printf("%s => %s\n", key, md5_16s(m, key));
    printf("%s => %s\n", key, md5_str(m, key));
}
