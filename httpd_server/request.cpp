#include "request.h"
#include "utils.h"

Request::Request() {
	this->client = NULL;
	this->method = "";
	this->url = "";
}

// 传入套接字，获取请求头信息
Request::Request(SOCKET clientSock) {
	this->setRequest(clientSock);
}

// 传入套接字，设置请求头信息
void Request::setRequest(SOCKET clientSock) {
	this->client = clientSock; // 保存socket-备用
	char buff[1024] = { 0 }; // 1K
	// 读取一行数据
	int numchars = get_line(clientSock, buff, sizeof(buff));
	
	int j = 0; // buff下标
	int i = 0; // method下标
	char method[255] = { 0 }; // 请求方法 GET or POST
	trimStart(buff, sizeof(buff), &j); // 跳过空格
	
	// 获取请求方法
	while (!isspace(buff[j]) && i < sizeof(method) - 1) {
		method[i++] = buff[j++];
	}
	method[i] = '\0';
	this->method = method;

	// 解析资源文件路径
	char url[255] = { 0 }; // 存放完整资源路径
	trimStart(buff, sizeof(buff), &j); // 跳过空格
	i = 0; // 重置
	while (
		j < sizeof(buff) &&
		i < sizeof(url) - 1 &&
		!isspace(buff[j])
		)
	{
		url[i++] = buff[j++];
	}
	url[i] = '\0';
	this->url = url;


	// 读取剩下的数据，设置请求头
	this->setHead(clientSock);
}

// 设置请求头
void Request::setHead(SOCKET clientSock)
{
	// Host: developer.mozilla.org
	// User - Agent : Mozilla / 5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko / 20100101 Firefox / 50.0
	// Accept : text / html, application / xhtml + xml, application / xml; q = 0.9, */*;q=0.8
	// Accept-Language: en-US,en;q=0.5
	// Accept-Encoding: gzip, deflate, br
	// Referer: https://developer.mozilla.org/testpage.html
	// Connection: keep-alive
	// Upgrade-Insecure-Requests: 1
	// If-Modified-Since: Mon, 18 Jul 2016 02:36:04 GMT
	//I f-None-Match: "c561c68d0ba92bbeb8b0fff2a9199f722e3a621a"
	// Cache-Control: max-age=0
	char buff[1024] = "Host: developer.mozilla.org"; // 1K
	char* outer_ptr = NULL;

	// 读取一行数据
	// int numchars = get_line(clientSock, buff, sizeof(buff));
	string line(buff);
	
	char* tmp = strtok_s(buff, ":", &outer_ptr);

	

}



// 获取请求信息
string Request::getRequest() {
	return string();
}

// 获取请求方法
string Request::getMethods()
{
	return this->method;
}

// 获取请求URL
string Request::getUrl()
{
	return this->url;
}

// 读取剩下的资源
void Request::release()
{
	char buff[1024] = { 0 };
	int numchars = 1;
	// 请求包的剩余数据读取完毕
	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(client, buff, sizeof(buff));
	}
}

// 释放资源
Request::~Request() {
	this->head.clear();
}
