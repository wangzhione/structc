#include <url.h>
#include <stdio.h>
#include <string.h>

// url_test - url 测试
void url_test(void) {
    char * nrl;
    int len, nen;
    const char * url; 
    
    url = "http://www.baidu.com/s?_\\ie=utf-8&f=8&tn=baidu&wd=临时邮箱";
    len = (int)strlen(url);
    nrl = url_encode(url, len, &nen);
    printf("url[len = %d]: %s\n", len, url);
    printf("nrl[nen = %d]: %s\n", nen, nrl);
    len = url_decode(nrl, nen);
    printf("nrl[len = %d]: %s\n", len, nrl);
    free(nrl);
}
