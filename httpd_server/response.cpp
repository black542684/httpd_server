#include "response.h"
#include <iostream>
#include <io.h>

/**
* request: ����
* folder: Ĭ���ļ���
*/
Response::Response(Request& request, string folder)
{
	this->client = request.getSocket(); // �ͻ����׽���
	this->folder = folder; // �����ļ��л���·��
	// this->gzip = request.isgzip(); // �Ƿ���gzip
	this->url = request.getUrl();
	
	generatePath(request.getUrl());
	generateMIME();
	generateHead();
}

// ������Ӧͷ
void Response::sendHeaders()
{
	// ��Ӧ��
	string responseLine = this->protocol + this->status_code;
	::send(this->client, responseLine.c_str(), responseLine.length(), 0);

	// ��Ӧͷ
	for (auto it = head.begin(); it != head.end(); it++) {
		string line(it->first + ": " + it->second + "\r\n");
		::send(this->client, line.c_str(), line.length(), 0);
	}
	string over = "\r\n";
	::send(this->client, over.c_str(), over.length(), 0);
}

// ������Դ�ļ�
void Response::snedFile(string filePath)
{
	FILE* resource = fopen(filePath.c_str(), "rb");
	// ��ָ���׽��֣�������Ӧ��
	char buff[4096] = { 0 };
	size_t count = 0; // ��¼��ȡ�����ֽ�

	while (true)
	{
		size_t ret = fread(buff, sizeof(char), sizeof(buff), resource);
		// û�ж�ȡ��
		if (ret <= 0) {
			break;
		}
		// ���׽��ַ�������
		::send(this->client, buff, ret, 0);
		count += ret;
	}

	printf("һ������%lld���ֽ�\n\n\n", count);
}

// ����gzip
void Response::sendFileGZIP()
{
	string filePath(this->path + ".gz");
	int flag = _access_s(filePath.c_str(), 0); // �ж� xxx.gz �ļ��Ƿ����
	// �ļ�������
	if (flag == ENOENT) {
		bool flag = fileToGZIP(this->path); //�ȴ����ļ�
		if (!flag) { // �����ļ�ʧ��
			filePath = this->path;
		}
	}

	snedFile(filePath); // �����ļ�

}

// ���ͻ��˷�����Դ
void Response::send()
{
	// ��ͻ��˷����ļ�
	int numchars = 1;
	char buff[1024] = { 0 };

	// �ļ�������
	if (this->res_is_empty == -1) {
		not_found(this->client);
		return;
	}

	this->sendHeaders();
	if (this->gzip) {
		this->sendFileGZIP();
	}
	else {
		this->snedFile(this->path);
	}
}

// ����Ĭ����Ӧͷ
void Response::generateHead()
{
	this->head["Server"] = "z-httpd/0.1";

	if (this->mime == "text/html") {
		this->head["Content-Type"] = "text/html; charset=utf-8";
	}
	else {
		this->head["Content-Type"] = this->mime;
	}

	// ֻ���ı��ļ��ſ���GZIP
	if (mime != "text/plain" && mime != "text/css" && mime != "text/javascript") {
		// printf("������gzip \n");
		this->gzip = false;
	}
	else {
		// printf("����gzip \n");
		if (this->gzip) { // �����֧��gzip�Ļ�
			this->head["Content-Encoding"] = "gzip";
		}
	}

}

// ���������ļ�·��
void Response::generatePath(string url)
{
	if (url == "/") {
		this->path = this->folder + "/index.html";
	}
	else {
		this->path = this->folder + url;
	}

	// �ļ�·��
	string path(this->path);

	// �ж�path ·��������-�ļ�or�ļ���
	struct stat status;
	this->res_is_empty = stat(path.c_str(), &status);

	// ������ļ���-��ƴ��  /docs/image   index.html
	if ((status.st_mode & S_IFMT) == S_IFDIR) {
		if (this->path[this->path.length() - 1] == '/') {
			this->path += "index.html";
		}
		else {
			this->path += "/index.html";
		}
	}


}

// ������������
void Response::generateMIME()
{
	string mime(getContentType(this->path.c_str()));
	this->mime = mime;
}
