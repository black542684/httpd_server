#pragma once
#ifndef RESPONSE
#define RESPONSE
#include "request.h"
#include "utils.h"
#include <unordered_map>

// 响应
class Response
{
private:
	// 响应头
	unordered_map<string, string> head; 
	bool gzip;
	string folder; // 文件夹
	string path; // 文件路径
	SOCKET client;// 套接字
	string mime; // content-type
	int res_is_empty; // 资源是否存在 -1 不存在
	

public:
	Response(Request& request, string folder);
	void send(); // 给客户端返回资源

private:
	void sendHeaders(); // 发送响应头
	void snedFile(FILE* resource); // 发送资源文件
	void generateHead(); // 生成默认响应头
	void generatePath(string url);  // 生成请求文件路径
	void generateMIME(); // 生成数据类型
};
#endif // !RESPONSE



