#pragma once
#ifndef REQUEST_HEAD
#define REQUEST_HEAD

#include <string>
#include <WinSock2.h>
#include <algorithm>
#include <stdexcept>
#include "utils.h"
#include "../nlohmann/json.hpp"
#include "../tinyxml2/tinyxml2.h"


using JSON = nlohmann::json;
using XML = tinyxml2::XMLDocument;

using namespace std;

/*
* ����������Ϣ
*/
class Request {
private:
	SOCKET client;// �ͻ����׽���
	unordered_map<string, string> head; // ����ͷ��Ϣ
	unordered_map<string, string> query; // �������
private:
	string method; // ���󷽷�
	string url; // ��Դ·��
private:
	unordered_map<string, string> body_form; // ����form���ύ��������
	JSON body_json; // ����json�ύ��������
	XML body_xml; // ����xml��������


public:
	Request();
	// �����׽��֣���ȡ����ͷ��Ϣ
	Request(SOCKET clientSock);
	// ����������Ϣ
	void setRequest(SOCKET clientSock);
	// ��������ͷ
	void setHead(SOCKET clientSock);
	// ����������
	void setBody(SOCKET clientSock);
	// ����form�����ύ
	void parseForm(string& body);
	// ����json��ʽ���ύ
	void parseJSON(string& body);
	// ����XML��ʽ���ύ
	void parseXML(string& body);
	// ����FormData��ʽ���ύ
	void parseFormData(vector<char>& body);

	
	// ��ȡ������Ϣ
	string getRequest();
	string getMethods();
	string getUrl();

	// ������Ƿ�֧��gzip
	bool isgzip(); 
	// ��ȡʣ�µ���Դ���ͷ��ڴ�-discard
	void release();

	// ��ȡsocket
	SOCKET getSocket();
	// �ͷ���Դ
	~Request();
};
#endif // !REQUEST_HEAD
