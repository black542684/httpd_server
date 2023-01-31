#include "request.h"

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
	this->client = clientSock; // ����socket- ����
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

	char* outer_ptr = NULL;
	// ��?�ָ��������   /api?name=zs&age=1
	char* tmp = strtok_s(url, "?", &outer_ptr);
	this->url = tmp; // ����·��

	// ��ȡ�������
	tmp = strtok_s(NULL, "?", &outer_ptr);
	if (tmp) {
		parseQuery(this->query, tmp);
	}


	// ��ȡʣ�µ����ݣ���������ͷ
	this->setHead(clientSock);
}

// ��������ͷ
void Request::setHead(SOCKET clientSock)
{
	// Host: developer.mozilla.org
	char buff[1024] = { 0 };
	char* outer_ptr = NULL;
	
	int numchars = 1;
	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(clientSock, buff, sizeof(buff));
		
		if (strcmp(buff, "\n")) { // ����������һ���ַ� '\n'
			char* tmp = strtok_s(buff, ":", &outer_ptr);
			string key(tmp);
			delete_space(key);
			transform(key.begin(), key.end(), key.begin(), ::tolower); // ȫ��ת��Сд
			

			tmp = strtok_s(NULL, ":", &outer_ptr);
			string value(tmp);
			delete_space(value);

			this->head[key] = value;
		}
	}	

	/*for (unordered_map<string, string>::iterator it = this->head.begin(); it != this->head.end(); it++) {
		printf("%s: ", it->first.c_str());
		printf("%s \n", it->second.c_str());
	}*/

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

// ������Ƿ�֧��gzip
bool Request::isgzip()
{
	bool flag = false;
	try
	{
		string value = this->head.at("accept-encoding");
		int index = value.find("gzip");
		if (index < value.length())
			flag = true;
		else
			flag = false;
	}
	catch (const std::out_of_range&)
	{
		flag = false;
	}
	
	
	return flag;
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

SOCKET Request::getSocket()
{
	return this->client;
}

// �ͷ���Դ
Request::~Request() {
	this->head.clear();
	this->query.clear();
}
