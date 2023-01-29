#pragma once
#ifndef UTILS
#define UTILS
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <stdexcept>
using namespace std;

/*
* 工具函数
*/

// 读取请求的一行
int get_line(SOCKET sock, char* buff, int size);

// 跳过空格
void trimStart(char* str, int size, int* index);

// 去除string 两端空格
void delete_space(std::string& s);

// 解析请求参数
void parseQuery(unordered_map<string, string> &query, char* queryStr);


// 获取MIME类型
char* getContentType(const char* path);



// 页面未找到
void not_found(SOCKET client);

#endif // !UTILS
