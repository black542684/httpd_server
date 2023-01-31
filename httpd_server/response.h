#pragma once
#ifndef RESPONSE
#define RESPONSE
#include "request.h"
#include "utils.h"
#include <unordered_map>

// ��Ӧ
class Response
{
private:
	// ��Ӧͷ
	unordered_map<string, string> head; 
	bool gzip;
	string folder; // �ļ���
	string path; // �ļ�·��
	SOCKET client;// �׽���
	string mime; // content-type
	int res_is_empty; // ��Դ�Ƿ���� -1 ������
	

public:
	Response(Request& request, string folder);
	void send(); // ���ͻ��˷�����Դ

private:
	void sendHeaders(); // ������Ӧͷ
	void snedFile(FILE* resource); // ������Դ�ļ�
	void generateHead(); // ����Ĭ����Ӧͷ
	void generatePath(string url);  // ���������ļ�·��
	void generateMIME(); // ������������
};
#endif // !RESPONSE



