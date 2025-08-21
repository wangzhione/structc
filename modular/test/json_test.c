#include <json.h>
#include <q.h>

void json_delete1(json_t cj) {
    if (cj == nullptr) {
        return;
    }

    struct q q; q_init(&q);

    do {
        json_t next = cj->next;
        unsigned type = cj->type;
        
        free(cj->key);
        if ((type & JSON_STRING) && !(type & JSON_CONST))
            free(cj->str);

        if (cj->child) {
            q_push(&q, cj->child);
        }

        free(cj);
        cj = next ? next : q_pop(&q);
    } while (cj);

    q_release(&q);
}

//
// json 功能测试
//
void json_test(void) {
    char c = -60;
    printf("c = %d = \\u%04x, sizeof char = %zu\n", (unsigned char)c, (unsigned char)c, sizeof c);

    const char * jstr = "{\"name\" : \"nihao\", \"nums\" : -123,  }";
    json_t r = json_create(jstr);
    if (nullptr == r) {
        fprintf(stderr, "json_create is err %s\n", jstr);
        return;
    }
    printf("r type = %d, len = %d\n", r->type, r->len);

    char * str = json_string(r);
    printf("jstr = %s\n", jstr);
    printf("str  = %s\n", str);
    free(str);
    json_delete1(r);
    
    // 第二轮开始测试
    jstr = "{\n\"name\": \"Jack (\\\"Bee\\\") 你好\", \n\"format\": {\"type\":[1, 3, 4, 5.66], \n\"height\":     1080, \n\"interlace\":  false}\n}";
    r = json_create(jstr);
    if (nullptr == r) {
        fprintf(stderr, "json_create is err jstr %s\n", jstr);
        return;
    }
    printf("r type = %d, len = %d\n", r->type, r->len);
    json_t format = json_object(r, "format");
    printf("format type = %d, len = %d\n", format->type, format->len);
    str = json_string(r);
    printf("str  = %s\n", str);
    free(str);
    json_delete(r);

    // 第三轮测试
    jstr = "123456.789";
    r = json_create(jstr);
    if (nullptr == r) {
        fprintf(stderr, "json_create is err jstr %s\n", jstr);
        return;
    }
    str = json_string(r);
    printf("str  = %s\n", str);
    free(str);
    json_delete(r);
}
