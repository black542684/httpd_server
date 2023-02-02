#pragma once
#ifndef HTTP_CLASS
#define HTTP_CLASS
#include <unordered_map>
#include "request.h"
#include "response.h"

using namespace std;
typedef void (*HandleFUNC)(Request&, Response&);

// 初始化HTTP服务器
class Http
{
public:
	~Http();
private:
	Http();
private:
	unordered_map<string, HandleFUNC> getMethods; // 保存GET处理函数
	unordered_map<string, HandleFUNC> postMethods; // 保存GET处理函数
public:
	// 存放处理GET请求的函数指针
	void get(string, HandleFUNC); 
	// 存放处理POST请求的函数指针
	void post(string, HandleFUNC); 
	// 监听端口
	void listen(unsigned short port);
private:

public:
	static Http* getInstance() { // 获取实例
		static Http ins;
		return &ins;
	}

};

Http* getHttp();

#endif // !HTTP_CLASS



