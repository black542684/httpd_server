#pragma once
#ifndef REQUEST_HEAD
#define REQUEST_HEAD

#include <string>
#include <WinSock2.h>
#include <unordered_map>

using namespace std;
/*
* 保存请求信息
*/
class Request {
private:
	SOCKET client;
	// 请求头信息
	unordered_map<string, string> head;
	
	string method; // 请求方法
	string url; // 资源路径

public:
	Request();
	// 传入套接字，获取请求头信息
	Request(SOCKET clientSock);
	// 设置请求信息
	void setRequest(SOCKET clientSock);
	// 设置请求头
	void setHead(SOCKET clientSock);

	// 获取请求信息
	string getRequest();
	string getMethods();
	string getUrl();

	
	// 读取剩下的资源，释放内存
	void release();

	// 释放资源
	~Request();
};
#endif // !REQUEST_HEAD
