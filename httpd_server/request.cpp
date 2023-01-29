#include "request.h"

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
	this->client = clientSock; // 保存socket- 备用
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

	char* outer_ptr = NULL;
	// 用?分割请求参数   /api?name=zs&age=1
	char* tmp = strtok_s(url, "?", &outer_ptr);
	this->url = tmp; // 请求路径

	// 获取请求参数
	tmp = strtok_s(NULL, "?", &outer_ptr);
	if (tmp) {
		parseQuery(this->query, tmp);
	}


	// 读取剩下的数据，设置请求头
	this->setHead(clientSock);
}

// 设置请求头
void Request::setHead(SOCKET clientSock)
{
	// Host: developer.mozilla.org
	char buff[1024] = { 0 };
	char* outer_ptr = NULL;
	
	int numchars = 1;
	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(clientSock, buff, sizeof(buff));
		
		if (strcmp(buff, "\n")) { // 如果不是最后一个字符 '\n'
			char* tmp = strtok_s(buff, ":", &outer_ptr);
			string key(tmp);
			delete_space(key);
			transform(key.begin(), key.end(), key.begin(), ::tolower); // 全部转成小写
			

			tmp = strtok_s(NULL, ":", &outer_ptr);
			string value(tmp);
			delete_space(value);

			this->head[key] = value;
		}
	}	

	/*for (unordered_map<string, string>::iterator it = this->head.begin(); it != this->head.end(); it++) {
		printf("%s: ", it->first.c_str());
		printf("%s \n", it->second.c_str());
	}*/

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

// 浏览器是否支持gzip
bool Request::isgzip()
{
	bool flag = false;
	try
	{
		string value = this->head.at("accept-encoding");
		int index = value.find("gzip");
		if (index < value.length())
			flag = true;
		else
			flag = false;
	}
	catch (const std::out_of_range&)
	{
		flag = false;
	}
	
	
	return flag;
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

SOCKET Request::getSocket()
{
	return this->client;
}

// 释放资源
Request::~Request() {
	this->head.clear();
	this->query.clear();
}
