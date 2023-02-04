#pragma once
#ifndef UTILS
#define UTILS
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>

using namespace std;

/*
* 工具函数
*/

// 获取网络错误消息
void getNetworkError();

// 读取请求的一行
int get_line(SOCKET sock, char* buff, int size);

/*
* 从vector<char>中读取一行
* @param body 请求体
* @param buff 存放读取到的内容
* @param index 开始读取的位置
* @return 最后读取到的位置
*/
int get_line(vector<char>& body, string& buff, int index);

/*
* 从vector<char>中读取一行，并保存在另外一个vector<char>中
* @param body 请求体
* @param buff 存放读取到的内容
* @param index 开始读取的位置
* @return 最后读取到的位置
*/
int get_PartBody_File(vector<char>& body, vector<char>& buff, int index);

/**
*  解析multipart/form-data  part头
* @param body 请求体
* @param currentIndex 当前读取到的位置
* @param buff 读取每一行
* @param partHead 存放part头的map对象
* @return 读取到的位置
*/
int parsePartHead(vector<char>& body, int currentIndex, string& buff, unordered_map<string, string>& partHead);

/**
* 解析multipart/form-data  part 二进制文件内容
* @param body 数据源
* @param buff 存放数据的
* @param index 开始读取的位置
* @param startFlag boundary分割标志
* @param endFlag boundary分割标志
* @return 读取结束的位置
*/
int parsePartBody_File(vector<char>& body, vector<char>& buff, int index, string& startFlag, string& endFlag);

// 读取请求体-文本
int get_body(SOCKET sock, int read_count,string& body);

// 读取请求体-二进制
int get_body(SOCKET sock, int read_count, vector<char>& body);

// 跳过空格
void trimStart(char* str, int size, int* index);

// 去除string 两端空格
void delete_space(string& s);

// 去除string 两端引号
void delete_quotation(string& s);

// 解析请求参数
void parseQuery(unordered_map<string, string> &query, char* queryStr);

// 获取MIME类型
char* getContentType(const char* path);

// 文件转成gzip
bool fileToGZIP(string filePath);

// 页面未找到
void not_found(SOCKET client);

// 打印map
void printMap(unordered_map<string, string> &map);

#endif // !UTILS
