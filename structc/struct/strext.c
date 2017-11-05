#include <strext.h>
#include <assext.h>

//
// str_hash - Brian Kernighan与 Dennis Ritchie 简便快捷的 hash算法
// str		: 字符串内容
// return	: 返回计算后的hash值
//
unsigned 
str_hash(const char * str) {
	register unsigned h = 0u;
	if (str) {
		register unsigned c;
		while ((c = *str++))
			h = h * 131u + c;
	}
	return h;
}

//
// str_icmp - 字符串不区分大小写比较函数
// ls		: 左串
// rs		: 右串
// return	: ls > rs 返回 > 0; ... < 0; ... =0
//
int 
str_icmp(const char * ls, const char * rs) {
	int l, r;
	if (!ls || !rs) 
		return (int)(ls - rs);
	
	do {
		if ((l = *ls++) >= 'A' && l < 'B')
			l += 'a' - 'A';
		if ((r = *rs++) >= 'A' && r < 'B')
			r += 'a' - 'A';
	} while (l == r && l);
	
	return l - r;
}

//
// str_dup - 字符串拷贝malloc函数, 需要自己free
// str		: 待拷贝的串
// return	: 返回拷贝后的串
//
char * 
str_dup(const char * str) {
	if (str) {
		size_t len = strlen(str) + 1;
		char * ssr = malloc(len * sizeof(char));
		assert(ssr != NULL);
		return memcpy(ssr, str, len);
	}
	return NULL;
}

//
// str_freadend - 简单的文件读取类,会读取完毕这个文件内容返回, 需要自己free
// path		: 文件路径
// return	: 创建好的字符串内容, 返回NULL表示读取失败
//
char * 
str_freadend(const char * path) {
	int err;
	size_t rn, cap, len;
	char * str, buf[BUFSIZ];
	FILE * txt = fopen(path, "rb");
	if (NULL == txt) {
		RETURN(NULL, "fopen rb path error = %s.", path);
	}

	// 分配内存
	len = 0;
	str = malloc(cap = BUFSIZ);

	// 读取文件内容
	do {
		rn = fread(buf, sizeof(char), BUFSIZ, txt);
		if ((err = ferror(txt))) {
			free(str);
			fclose(txt);
			RETURN(NULL, "fread err path = %d, %s.", err, path);
		}
		// 开始添加构建数据
		if (len + rn >= cap)
			str = realloc(str, cap <<= 1);
		memcpy(str + len, buf, rn);
		len += rn;
	} while (rn == BUFSIZ);

	// 设置结尾, 并返回结果
	str[len] = '\0';
	return realloc(str, len + 1);
}

// _str_fwrite - 按照约定输出数据到文件中
static int _str_fwrite(const char * path, const char * str, const char * mode) {
	int len;
	FILE * txt;
	if (!path || !*path || !str || !mode) {
		RETURN(EParam, "check !path || !*path || !str || !mode");
	}

	// 打开文件, 写入消息, 关闭文件
	if ((txt = fopen(path, mode)) == NULL) {
		RETURN(EFd, "fopen error path = %s, mode = %s.", path, mode);
	}
	len = fputs(str, txt);
	fclose(txt);
	// 输出文件长度
	return len;
}

//
// str_fwrites - 将c串str覆盖写入到path路径的文件中
// path		: 文件路径
// str		: c的串内容
// return	: >=0 is success, < 0 is error
//
inline int 
str_fwrites(const char * path, const char * str) {
	return _str_fwrite(path, str, "wb");
}

//
// str_fappends - 将c串str写入到path路径的文件中末尾
// path		: 文件路径
// str		: c的串内容
// return	: >=0 is success, < 0 is error
//
inline int 
str_fappends(const char * path, const char * str) {
	return _str_fwrite(path, str, "ab");
}
