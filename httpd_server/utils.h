#pragma once
#ifndef UTILS
#define UTILS
#include <WinSock2.h>
#include <stdio.h>
/*
* ���ߺ���
*/

// ��ȡ�����һ��
int get_line(SOCKET sock, char* buff, int size);

// �����ո�
void trimStart(char* str, int size, int* index);

#endif // !UTILS
