#pragma once
#ifndef RESPONSE
#define RESPONSE
#include "request.h"
#include "utils.h"

// 响应
class Response
{
private:
	// 响应行
	string protocol = "HTTP/1.1 "; // 协议和版本
	string status_code = "200 OK\r\n"; // 状态码和状态描述														 
	unordered_map<string, string> head; // 响应头

private:
	bool gzip;
	string url;// 请求路径
	string folder; // 文件夹
	string path; // 文件路径
	SOCKET client;// 套接字
	string mime; // content-type
	int res_is_empty; // 资源是否存在 -1 不存在
	

public:
	Response();
	Response(Request& request, string folder);
	void send(); // 给客户端返回资源
	void send(string); // 给客户端返回字符串
	void header(string key, string value); // 设置响应头
	unordered_map<string, string> getHeader(); // 返回响应头
	void setGZIP(bool); // 设置gzip

private:
	void sendHeaders(); // 发送响应头
	void snedFile(string filePath); // 发送资源文件
	void sendFileGZIP(); // 发送GZIP资源文件
	void sendText(string);// 发送文本
	
	void generateHead(); // 生成默认响应头
	void generatePath(string url);  // 生成请求文件路径
	void generateMIME(); // 生成数据类型
};
#endif // !RESPONSE



