#pragma once
#ifndef REQUEST_HEAD
#define REQUEST_HEAD

#include <string>
#include <WinSock2.h>
#include <unordered_map>

using namespace std;
/*
* ����������Ϣ
*/
class Request {
private:
	SOCKET client;
	// ����ͷ��Ϣ
	unordered_map<string, string> head;
	
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

	
	// ��ȡʣ�µ���Դ���ͷ��ڴ�
	void release();

	// �ͷ���Դ
	~Request();
};
#endif // !REQUEST_HEAD
