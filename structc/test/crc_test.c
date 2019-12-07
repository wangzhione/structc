#include <crc.h>
#include <base.h>

void crc_test(void) {
    const char * data = "123";
    uint16_t crc;

    crc = crc16(data, strlen(data));
    printf("crc = %hu\n", crc);

    
}
