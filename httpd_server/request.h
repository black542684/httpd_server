#pragma once
#ifndef REQUEST_HEAD
#define REQUEST_HEAD

#include <string>
#include <WinSock2.h>
#include <algorithm>
#include <stdexcept>
#include "utils.h"
#include "FormDataField.h"
#include "FormDataFile.h"
#include "../nlohmann/json.hpp"
#include "../tinyxml2/tinyxml2.h"


using JSON = nlohmann::json;
using XML = tinyxml2::XMLDocument;

using namespace std;

/*
* 保存请求信息
*/
class Request {
private:
	SOCKET client;// 客户端套接字
	unordered_map<string, string> head; // 请求头信息
	unordered_map<string, string> query; // 请求参数
private:
	string method; // 请求方法
	string url; // 资源路径
public:
	string path; // 请求路径

private:
	unordered_map<string, string> body_form; // 保存form表单提交的请求体
	JSON body_json; // 保存json提交的请求体
	XML body_xml; // 保存xml的请求体
	
	/* form-data */
	vector<FormDataField> fields; // 参数对象
	vector<FormDataFile> files; // 文件对象 

public:
	Request();
	// 传入套接字，获取请求头信息
	Request(SOCKET clientSock);
	// 设置请求信息
	void setRequest(SOCKET clientSock);
	// 设置请求头
	void setHead(SOCKET clientSock);
	// 设置请求体
	void setBody(SOCKET clientSock);
	// 解析form表单的提交
	void parseForm(string& body);
	// 解析json格式的提交
	void parseJSON(string& body);
	// 解析XML格式的提交
	void parseXML(string& body);
	// 解析FormData格式的提交
	void parseFormData(vector<char>& body);

	
	// 获取请求信息
	string getRequest();
	string getMethods();
	string getUrl();

	// 浏览器是否支持gzip
	bool isgzip(); 
	// 读取剩下的资源，释放内存-discard
	void release();

	// 获取socket
	SOCKET getSocket();
	// 释放资源
	~Request();
};
#endif // !REQUEST_HEAD
