#pragma once
#ifndef UTILS
#define UTILS
#include <WinSock2.h>
#include <stdio.h>
/*
* 工具函数
*/

// 读取请求的一行
int get_line(SOCKET sock, char* buff, int size);

// 跳过空格
void trimStart(char* str, int size, int* index);

#endif // !UTILS
