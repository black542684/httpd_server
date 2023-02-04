#include "request.h"
#include <iostream>

Request::Request() {
	this->client = NULL;
	this->method = "";
	this->url = "";
}

// 传入套接字，获取请求信息
Request::Request(SOCKET clientSock) {
	this->setRequest(clientSock);
}

// 传入套接字，读取请求信息
void Request::setRequest(SOCKET clientSock) {
	this->client = clientSock; // 保存socket- 备用
	char buff[1024] = { 0 }; // 1K
	
	// 读取一行数据
	int numchars = get_line(clientSock, buff, sizeof(buff) - 1);
	
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
	printf("\n请求方法：%s\n", this->method.c_str());

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
		numchars = get_line(clientSock, buff, sizeof(buff) - 1);

		if (numchars > 1 && strcmp(buff, "\n")) { // 如果不是最后一个字符 '\n'

			char* tmp = strtok_s(buff, ":", &outer_ptr); // 用 : 分割   Content-tye: text/plain
			string key(tmp);
			delete_space(key); // 去除两端空格
			
			tmp = strtok_s(NULL, ":", &outer_ptr);
			string value(tmp);
			delete_space(value); // 去除两端空格
			
			if (!key.empty() && !value.empty()) {
				this->head.emplace(key, value);
			}
		}
	}	
	
	if (this->method == "POST") {
		//这是里post请求
		setBody(clientSock);
	}
	
	

}

// 设置请求体
void Request::setBody(SOCKET clientSock)
{
	string length = this->head["Content-Length"]; // 请求体长度
	string body; // 临时保存请求体-文本文件
	vector<char> body_binary;

	int len = atoi(length.c_str()); // string-转-int
	if (len <= 0) return;

	string contentType = this->head["Content-Type"];
	cout << "contentType:" << contentType << endl;
	if (contentType.find("multipart/form-data") != -1) {
		get_body(clientSock, len, body_binary);
	}
	else {
		get_body(clientSock, len, body);
	}


	if (contentType.find("application/x-www-form-urlencoded") != -1) {
		parseForm(body);
	}
	else if (contentType.find("application/json") != -1) {
		parseJSON(body);
	}
	else if (contentType.find("text/xml") != -1) {
		parseXML(body);
	}
	else if (contentType.find("multipart/form-data") != -1) {
		parseFormData(body_binary);
	}
}

// // 解析form表单的提交
void Request::parseForm(string &body)
{
	char* str = (char*)body.c_str();
	parseQuery(this->body_form, str); // 解析form表单，并保存
}

// 解析json格式的提交
void Request::parseJSON(string& body)
{
	this->body_json = JSON::parse(body);
	for (JSON::iterator it = this->body_json.begin(); it != this->body_json.end(); ++it) {
		std::cout << it.key() << " : " << it.value() << "\n";
	}
}

// 解析XML格式的提交
void Request::parseXML(string& body)
{
	this->body_xml.Parse(body.c_str());
}

// 解析FormData格式的提交
void Request::parseFormData(vector<char>& body)
{
	string contentType = this->head["Content-Type"];
	// ; 分割 multipart/form-data; boundary=----WebKitFormBoundary5On24LeqJdcOBcBC
	size_t index = contentType.find("=");
	if (index == -1) return;
	
	

	string boundary = contentType.substr(index + 1);
	string start = "--" + boundary; // 每一个数据段开始位置
	string end = start + "--"; // 数据段结束位置

	string buff; // 文本缓存
	vector<char> part_buff;// 二进制缓存

	int startindex = 0; // 读取到的位置
	unordered_map<string, string> partHead; // 存放part头
	

	while (true)
	{
		buff.clear();
		startindex = get_line(body, buff, startindex);
		// 读取到开始区域
		if (buff == start) {
			buff.clear();
			startindex = parsePartHead(body, startindex, buff, partHead);
		}
		else if (buff == end) {// 读取到了结束标志
			buff.clear();
			break;
		}
		
		if (buff.size() == 0) { // 读取内容区域 { buff.size() == 0 表示读到了\r\n }
			buff.clear();

			// 判断是文本还是 文件
			if (partHead.count("filename") > 0) { // 如果存在filename字段，说明是一个文件
				/* 第一种读取方式 */
				// startindex = parsePartBody_File(body, part_buff, startindex, start, end);
				/* 第二种读取方式 */
				startindex = get_PartBody_File(body, part_buff, startindex);
				cout << "文件:" << part_buff.size() << endl;
			}
			else {
				startindex = get_line(body, buff, startindex);
				cout << "不是文件:" << buff << endl;
			}
		}
	}


}






// 获取请求信息
string Request::getRequest()
{
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
		string value = this->head.at("Accept-Encoding");
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
		printf("[%s] \n", buff);
		numchars = get_line(client, buff, sizeof(buff) - 1);
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
