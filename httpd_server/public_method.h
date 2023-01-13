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
* ��ӡ������Ϣ���˳�����
* @param str ������ʾ
*/
void error_die(const char* str);
/**
* ����ͨ�ų�ʼ��
* @param port
* @return socket
*/
SOCKET startup(unsigned short* port);

// �����������
void unimplement(SOCKET client);

// û���ҵ�ҳ��
void not_found(SOCKET client);

// ��ȡ��Դ����
char* getContentType(const char* path);

// ������Ӧͷ
void headers(SOCKET client, const char* type);

// �����������Դ��Ϣ
void cat(SOCKET client, FILE* resource);

// ��ͻ��˷���һ��ҳ��
void server_file(SOCKET client, const char* path);

// �����û�������߳�
unsigned WINAPI accept_request(void* arg);


#endif // !PUBLIC_METHOD