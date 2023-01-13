#pragma once
#ifndef PUBLIC_METHOD
#define PUBLIC_METHOD
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <WinSock2.h>

using namespace std;

#pragma comment(lib, "WS2_32.lib")

#define PRINTF(str) printf("[%s - %d] "#str" = %s \n", __func__, __LINE__, str);

/**
* 打印错误信息并退出程序
* @param str 错误提示
*/
void error_die(const char* str);
/**
* 网络通信初始化
* @param port
* @return socket
*/
SOCKET startup(unsigned short* port);

// 处理错误请求
void unimplement(SOCKET client);

// 没有找到页面
void not_found(SOCKET client);

// 获取资源类型
char* getContentType(const char* path);

// 发送响应头
void headers(SOCKET client, const char* type);

// 发送请求的资源信息
void cat(SOCKET client, FILE* resource);

// 向客户端发送一个页面
void server_file(SOCKET client, const char* path);

// 处理用户请求的线程
unsigned WINAPI accept_request(void* arg);


#endif // !PUBLIC_METHOD