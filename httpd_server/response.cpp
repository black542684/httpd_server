#include "response.h"

/**
* request: ����
* folder: Ĭ���ļ���
*/
Response::Response(Request& request, string folder)
{
	this->client = request.getSocket();
	this->folder = folder; // �����ļ��л���·��
	this->gzip = request.isgzip(); // �Ƿ���gzip

	
	generatePath(request.getUrl());
	generateMIME();
	
	//if (mime == "text/html") { // HTML�ļ�����gzip
	//	this->gzip = false;
	//}
	//else {
	//	this->gzip = request.isgzip(); // �Ƿ���gzip
	//}

	generateHead();
}

// ������Ӧͷ
void Response::sendHeaders()
{
	// ��ָ���׽��֣�����һ����Ӧͷ
	string responseLine = "HTTP/1.1 200 OK\r\n";
	::send(this->client, responseLine.c_str(), responseLine.length(), 0);

	for (auto it = head.begin(); it != head.end(); it++) {
		string line(it->first + ": " + it->second + "\r\n");
		::send(this->client, line.c_str(), line.length(), 0);
	}
	string over = "\r\n";
	::send(this->client, over.c_str(), over.length(), 0);
}

// ������Դ�ļ�
void Response::snedFile(FILE* resource)
{
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

// ���ͻ��˷�����Դ
void Response::send()
{
	// ��ͻ��˷����ļ�
	int numchars = 1;
	char buff[1024] = { 0 };

	// �ļ�������
	if (this->res_is_empty == -1) {
		not_found(this->client);
	}
	else {
		FILE* resource = fopen(this->path.c_str(), "rb");
		this->sendHeaders();
		this->snedFile(resource);
	}
}

// ����Ĭ����Ӧͷ
void Response::generateHead()
{
	this->head["Server"] = "z-httpd/1.1";

	if (this->mime == "text/html") {
		this->head["Content-Type"] = "text/html; charset=utf-8";
	}
	else {
		this->head["Content-Type"] = this->mime;
	}

	if (this->gzip) {
		this->head["Content-Encodin"] = "gzip";
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
	// �ж�path ·��������-�ļ�or�ļ���
	struct stat status;
	this->res_is_empty = stat(this->path.c_str(), &status);

	// �ж��ļ�����
	if ((status.st_mode & S_IFMT) == S_IFDIR) {
		
		// ������ļ���-��ƴ��  /docs/image index.html
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
