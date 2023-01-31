#pragma once
#ifndef REQUEST_HEAD
#define REQUEST_HEAD

#include <string>
#include <WinSock2.h>
#include <algorithm>
#include <stdexcept>
#include "utils.h"

using namespace std;
/*
* ����������Ϣ
*/
class Request {
private:
	SOCKET client;
	
	unordered_map<string, string> head; // ����ͷ��Ϣ
	unordered_map<string, string> query; // �������
	
	string method; // ���󷽷�
	string url; // ��Դ·��

public:
	Request();
	// �����׽��֣���ȡ����ͷ��Ϣ
	Request(SOCKET clientSock);
	// ����������Ϣ
	void setRequest(SOCKET clientSock);
	// ��������ͷ
	void setHead(SOCKET clientSock);

	// ��ȡ������Ϣ
	string getRequest();
	string getMethods();
	string getUrl();

	bool isgzip(); // ������Ƿ�֧��gzip
	
	// ��ȡʣ�µ���Դ���ͷ��ڴ�
	void release();

	// ��ȡsocket
	SOCKET getSocket();

	// �ͷ���Դ
	~Request();
};
#endif // !REQUEST_HEAD
