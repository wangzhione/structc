#include <check.h>

//
// is_ip - 判断是否是 ip
// ips      : ip 串
// return   : true 是合法 ip
//
bool 
is_ip(const char * ips) {
    //
    // [0-9].
    // 7       - 15
    // 0.0.0.0 - 255.255.255.255
    // 00 (x), > 255 (x), . . .
    //
    int i, c, m, d;
    if (!ips || !*ips) return false;
    // 处理前 16 个字符 = sizeof "255.255.255.255"
    for (d = m = i = 0; i < sizeof "255.255.255.255"; ++i) {
        c = ips[i];
        if (c >= '0' && c <= '9') {
            // 00 (x)
            if (c == '0') {
                if (ips[i+1] != '.' && ips[i+1] != '\0')
                    return false;
            }
            m = m * 10 + c - '0';
        } else if (c == '.' || c == '\0') {
            // < 0 (x), > 255 (x)
            if (m > 255 || d > 3)
                return false;
            if (c == '\0') {
                // . . .
                // sizeof "0.0.0.0" = 8
                return d == 3 && i + 1 >= sizeof "0.0.0.0";
            }
            ++d;
            m = 0;
        } else {
            // 不是 [0-9]. 字符非法
            return false;
        }
    }
    return false;
}
