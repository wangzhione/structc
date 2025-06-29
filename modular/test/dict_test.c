#include <dict.h>
#include <strext.h>

extern void dict_move_filter(dict_t d, dict_t a, void * filter);

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

    dict_set(d, k, nullptr);
    val = dict_get(d, k);
    printf("key = %s, val = %s\n", k, val);

    dict_t a = dict_create(str_die);
    while (i < 200) {
        val = str_sprintf("youto_%d", i);
        snprintf(key, sizeof key, "hello_%d", i);
        dict_set(a, key, val);

        ++i;
    }

    dict_move_filter(d, a, nullptr);

    printf("d = %p, a = %p\n", d, a);

    val = dict_get(d, k = "hello_109");
    printf("key = %s, val = %s\n", k, val);

    val = dict_get(d, k = "hello_199");
    printf("key = %s, val = %s\n", k, val);

    dict_delete(d);
}
