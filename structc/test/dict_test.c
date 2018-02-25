#include <dict.h>

static void _str_die(void * node) {
    free(node);
}

//
// dict test heoo
//
void dict_test(void) {
    unsigned i;
    char * val;
    char * k, key[100];
    dict_t d = dict_create(_str_die);

    for (i = 0; i < 100; ++i) {
        val = str_printf("youto_%d", i);
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

    dict_delete(d);
}
