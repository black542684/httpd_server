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
	string staticPath = "./"; // 静态资源存放路径
	unordered_map<string, HandleFUNC> allMethods; // 保存all处理函数
	unordered_map<string, HandleFUNC> getMethods; // 保存GET处理函数
	unordered_map<string, HandleFUNC> postMethods; // 保存GET处理函数
public:
	Request req; // 请求对象
	Response res; // 响应对象

public:
	// 设置静态资源存放路径
	void setStatic(string path);
	string getStatic();

	// 存放处理所有请求的函数指针
	void all(string, HandleFUNC);
	HandleFUNC all(string); // 获取处理所有请求的函数指针

 	// 存放处理GET请求的函数指针
	void get(string, HandleFUNC); 
	HandleFUNC get(string);

	// 存放处理POST请求的函数指针
	void post(string, HandleFUNC); 
	HandleFUNC post(string);


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



