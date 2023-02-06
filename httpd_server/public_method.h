#pragma once
#ifndef PUBLIC_METHOD
#define PUBLIC_METHOD
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <WinSock2.h>
#include <process.h>

#include "Http.h"
#include "request.h"
#include "response.h"

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
* @param port 端口号
* @return socket 套接字
*/
SOCKET startup(unsigned short* port);

// 处理错误请求
void unimplement(SOCKET client);

// 处理用户请求的线程
unsigned WINAPI accept_request(void* arg);

// 监听套接字
void listenSocket(unsigned short port);

// 处理OPTIONS请求
void handle_options(SOCKET, Response&);


#endif // !PUBLIC_METHOD