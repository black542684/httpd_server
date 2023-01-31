#pragma once
#ifndef RESPONSE
#define RESPONSE
#include "request.h"
#include "utils.h"

// ��Ӧ
class Response
{
private:
	// ��Ӧ��
	string protocol = "HTTP/1.1 "; // Э��Ͱ汾
	string status_code = "200 OK\r\n"; // ״̬���״̬����
	// ��Ӧͷ
	unordered_map<string, string> head; 
private:
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
	void snedFile(string filePath); // ������Դ�ļ�
	void sendFileGZIP(); // ����GZIP��Դ�ļ�
	void generateHead(); // ����Ĭ����Ӧͷ
	void generatePath(string url);  // ���������ļ�·��
	void generateMIME(); // ������������
};
#endif // !RESPONSE



