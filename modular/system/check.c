#include "check.h"

//
// is_email - 判断是否是邮箱
// mail     : email 串 
// return   : true is email
#define EMAIL_INT (255)
bool 
is_email(const char * mail) {
    //
    // A@B.xx size <= 255
    // [0-9][a-z][A-Z] .-_ 字符构成
    // @ . 分割处首位和末位只能是 [0-9][a-z][A-Z] 
    // 不能出现 .. 连续
    //
    int c, i, b, d;
    if (!mail) return false;
    // check A
    c = *mail;
    if (!((c >= '0' && c <= '9') 
       || (c >= 'A' && c <= 'Z') 
       || (c >= 'a' && c <= 'z'))) return false;
    for (i = 1; (c = *++mail) && c != '@' && i < EMAIL_INT; ++i) {
        // 非法字符直接返回
        if (!((c >= '0' && c <= '9') 
        || (c >= 'A' && c <= 'Z') 
        || (c >= 'a' && c <= 'z'))) {
            if (c == '-' || c == '_')
                continue;
            return false;
        }
    }

    // check A end
    if (c != '@' || i >= EMAIL_INT 
                 || mail[-1] == '-' || mail[-1] == '_')
        return false;
    
    // check b start
    for (b = d = false; (c = *++mail) && i < EMAIL_INT; ++i) {
        // 非法字符直接返回
        if (!((c >= '0' && c <= '9') 
        || (c >= 'A' && c <= 'Z') 
        || (c >= 'a' && c <= 'z'))) {
            // 首字符不能是 非数字和字母
            if (b) {
                if (c == '-' || c == '_') 
                    continue;
                else if (c == '.') {
                    b = false;
                    d = true;
                    continue;
                }
            }
            return false;
        }
        b = true;
    }
    // 必须存在 ., 最后 '\0' 结尾, 255 以内
    return b && d && !c && i < EMAIL_INT 
             && (mail[-1] < '0' || mail[-1] > '9');
}
