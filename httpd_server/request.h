#pragma once
#ifndef REQUEST_HEAD
#define REQUEST_HEAD

#include <string>
#include <WinSock2.h>
#include <algorithm>
#include <stdexcept>
#include "utils.h"

using namespace std;
/*
* 保存请求信息
*/
class Request {
private:
	SOCKET client;
	
	unordered_map<string, string> head; // 请求头信息
	unordered_map<string, string> query; // 请求参数
	
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

	bool isgzip(); // 浏览器是否支持gzip
	
	// 读取剩下的资源，释放内存
	void release();

	// 获取socket
	SOCKET getSocket();

	// 释放资源
	~Request();
};
#endif // !REQUEST_HEAD
