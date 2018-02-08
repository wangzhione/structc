#include <json.h>

//
// json 功能测试
//
void json_test(void) {
    const char * jstr = "{\"name\" : \"nihao\", \"nums\" : -123,  }";
    json_t r = json_create(jstr);
    if (NULL == r) {
        fprintf(stderr, "json_create is err %s\n", jstr);
        return;
    }
    printf("r type = %d\n", r->type);
    json_delete(r);
}
