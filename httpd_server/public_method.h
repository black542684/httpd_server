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
* ��ӡ������Ϣ���˳�����
* @param str ������ʾ
*/
void error_die(const char* str);
/**
* ����ͨ�ų�ʼ��
* @param port �˿ں�
* @return socket �׽���
*/
SOCKET startup(unsigned short* port);

// �����������
void unimplement(SOCKET client);

// �����û�������߳�
unsigned WINAPI accept_request(void* arg);

// �����׽���
void listenSocket(unsigned short port);

// ����OPTIONS����
void handle_options(SOCKET, Response&);


#endif // !PUBLIC_METHOD