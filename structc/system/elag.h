#ifndef _H_ELAG
#define _H_ELAG

//
// enum Elag int - 函数返回值全局状态码
// >= 0 标识Success状态, < 0 标识Error状态
//
enum Elag {
	EParse    = -8, //协议解析错误
	EClose    = -7, //句柄打开失败, 读取完毕也返回这个
	EEmpty    = -6, //返回数据为空
	ETout     = -5, //操作超时错误
	EFd       = -4, //文件打开失败
	EAlloc    = -3, //内存分配错误
	EParam    = -2, //输入参数错误
	EBase     = -1, //错误基础类型, 所有位置错误都可用它

	SBase     = +0, //基础正确类型
};

#endif//_H_ELAG
