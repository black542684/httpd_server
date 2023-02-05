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
	unordered_map<string, string> head; // ��Ӧͷ

private:
	bool gzip;
	string url;// ����·��
	string folder; // �ļ���
	string path; // �ļ�·��
	SOCKET client;// �׽���
	string mime; // content-type
	int res_is_empty; // ��Դ�Ƿ���� -1 ������
	

public:
	Response();
	Response(Request& request, string folder);
	void send(); // ���ͻ��˷�����Դ
	void send(string); // ���ͻ��˷����ַ���
	void header(string key, string value); // ������Ӧͷ
	unordered_map<string, string> getHeader(); // ������Ӧͷ
	void setGZIP(bool); // ����gzip

private:
	void sendHeaders(); // ������Ӧͷ
	void snedFile(string filePath); // ������Դ�ļ�
	void sendFileGZIP(); // ����GZIP��Դ�ļ�
	void sendText(string);// �����ı�
	
	void generateHead(); // ����Ĭ����Ӧͷ
	void generatePath(string url);  // ���������ļ�·��
	void generateMIME(); // ������������
};
#endif // !RESPONSE



