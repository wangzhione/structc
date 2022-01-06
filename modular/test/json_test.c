#include <json.h>

//
// json 功能测试
//
void json_test(void) {
    char c = -60;
    printf("c = %d = \\u%04x, sizeof char = %zu\n", (unsigned char)c, (unsigned char)c, sizeof c);

    const char * jstr = "{\"name\" : \"nihao\", \"nums\" : -123,  }";
    json_t r = json_create(jstr);
    if (NULL == r) {
        fprintf(stderr, "json_create is err %s\n", jstr);
        return;
    }
    printf("r type = %d\n", r->type);

    char * str = json_print(r);
    printf("jstr = %s\n", jstr);
    printf("str  = %s\n", str);
    free(str);
    json_delete(r);
    
    // 第二轮开始测试
    jstr = "{\n\"name\": \"Jack (\\\"Bee\\\") 你好\", \n\"format\": {\"type\":[1, 3, 4, 5.66], \n\"height\":     1080, \n\"interlace\":  false}\n}";
    r = json_create(jstr);
    if (NULL == r) {
        fprintf(stderr, "json_create is err jstr %s\n", jstr);
        return;
    }
    str = json_print(r);
    printf("str  = %s\n", str);
    free(str);
    json_delete(r);
}
