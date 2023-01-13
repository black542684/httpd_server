#include "request.h"
#include "utils.h"

Request::Request() {
	this->client = NULL;
	this->method = "";
	this->url = "";
}

// �����׽��֣���ȡ����ͷ��Ϣ
Request::Request(SOCKET clientSock) {
	this->setRequest(clientSock);
}

// �����׽��֣���������ͷ��Ϣ
void Request::setRequest(SOCKET clientSock) {
	this->client = clientSock; // ����socket-����
	char buff[1024] = { 0 }; // 1K
	// ��ȡһ������
	int numchars = get_line(clientSock, buff, sizeof(buff));
	
	int j = 0; // buff�±�
	int i = 0; // method�±�
	char method[255] = { 0 }; // ���󷽷� GET or POST
	trimStart(buff, sizeof(buff), &j); // �����ո�
	
	// ��ȡ���󷽷�
	while (!isspace(buff[j]) && i < sizeof(method) - 1) {
		method[i++] = buff[j++];
	}
	method[i] = '\0';
	this->method = method;

	// ������Դ�ļ�·��
	char url[255] = { 0 }; // ���������Դ·��
	trimStart(buff, sizeof(buff), &j); // �����ո�
	i = 0; // ����
	while (
		j < sizeof(buff) &&
		i < sizeof(url) - 1 &&
		!isspace(buff[j])
		)
	{
		url[i++] = buff[j++];
	}
	url[i] = '\0';
	this->url = url;


	// ��ȡʣ�µ����ݣ���������ͷ
	this->setHead(clientSock);
}

// ��������ͷ
void Request::setHead(SOCKET clientSock)
{
	// Host: developer.mozilla.org
	// User - Agent : Mozilla / 5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko / 20100101 Firefox / 50.0
	// Accept : text / html, application / xhtml + xml, application / xml; q = 0.9, */*;q=0.8
	// Accept-Language: en-US,en;q=0.5
	// Accept-Encoding: gzip, deflate, br
	// Referer: https://developer.mozilla.org/testpage.html
	// Connection: keep-alive
	// Upgrade-Insecure-Requests: 1
	// If-Modified-Since: Mon, 18 Jul 2016 02:36:04 GMT
	//I f-None-Match: "c561c68d0ba92bbeb8b0fff2a9199f722e3a621a"
	// Cache-Control: max-age=0
	char buff[1024] = "Host: developer.mozilla.org"; // 1K
	char* outer_ptr = NULL;

	// ��ȡһ������
	// int numchars = get_line(clientSock, buff, sizeof(buff));
	string line(buff);
	
	char* tmp = strtok_s(buff, ":", &outer_ptr);

	

}



// ��ȡ������Ϣ
string Request::getRequest() {
	return string();
}

// ��ȡ���󷽷�
string Request::getMethods()
{
	return this->method;
}

// ��ȡ����URL
string Request::getUrl()
{
	return this->url;
}

// ��ȡʣ�µ���Դ
void Request::release()
{
	char buff[1024] = { 0 };
	int numchars = 1;
	// �������ʣ�����ݶ�ȡ���
	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(client, buff, sizeof(buff));
	}
}

// �ͷ���Դ
Request::~Request() {
	this->head.clear();
}
