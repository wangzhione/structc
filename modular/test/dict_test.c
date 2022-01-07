#include <dict.h>
#include <strext.h>

static void str_die(void * node) {
    free(node);
}

//
// dict test heoo
//
void dict_test(void) {
    unsigned i;
    char * val;
    char * k, key[100];
    dict_t d = dict_create(str_die);

    for (i = 0; i < 100; ++i) {
        val = str_sprintf("youto_%d", i);
        snprintf(key, sizeof key, "hello_%d", i);
        dict_set(d, key, val);
    }

    val = dict_get(d, k = "hello_99");
    printf("key = %s, val = %s\n", k, val);

    val = dict_get(d, k = "hello_1");
    printf("key = %s, val = %s\n", k, val);

    dict_set(d, k, NULL);
    val = dict_get(d, k);
    printf("key = %s, val = %s\n", k, val);

    dict_t a = dict_create(str_die);
    while (i < 200) {
        val = str_sprintf("youto_%d", i);
        snprintf(key, sizeof key, "hello_%d", i);
        dict_set(a, key, val);

        ++i;
    }

    dict_add_delete(&d, &a);

    printf("d = %p, a = %p\n", d, a);

    val = dict_get(d, k = "hello_109");
    printf("key = %s, val = %s\n", k, val);

    val = dict_get(d, k = "hello_199");
    printf("key = %s, val = %s\n", k, val);

    dict_delete(d);
}
