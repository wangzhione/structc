/*
gcc 
    __attribute__((__packed__)) 
cl  
    #pragma pack(push, 1) 
    #pragma pack(pop)

Example:
    #include "packed$begin.h"
    struct __attribute__((__packed__)) sdshdr5 {
        unsigned char flags;
        char buf[];
    };
    #include "packed$end.h"
 */

#if _MSC_VER

#pragma pack(pop)

#endif /* _MSC_VER */
