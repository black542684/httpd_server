#include "request.h"
#include <iostream>

Request::Request() {
	this->client = NULL;
	this->method = "";
	this->url = "";
}

// �����׽��֣���ȡ������Ϣ
Request::Request(SOCKET clientSock) {
	this->setRequest(clientSock);
}

// �����׽��֣���ȡ������Ϣ
void Request::setRequest(SOCKET clientSock) {
	this->client = clientSock; // ����socket- ����
	char buff[1024] = { 0 }; // 1K
	
	// ��ȡһ������
	int numchars = get_line(clientSock, buff, sizeof(buff) - 1);
	
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
	printf("\n���󷽷���%s\n", this->method.c_str());

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
		numchars = get_line(clientSock, buff, sizeof(buff) - 1);

		if (numchars > 1 && strcmp(buff, "\n")) { // ����������һ���ַ� '\n'

			char* tmp = strtok_s(buff, ":", &outer_ptr); // �� : �ָ�   Content-tye: text/plain
			string key(tmp);
			delete_space(key); // ȥ�����˿ո�
			
			tmp = strtok_s(NULL, ":", &outer_ptr);
			string value(tmp);
			delete_space(value); // ȥ�����˿ո�
			
			if (!key.empty() && !value.empty()) {
				this->head.emplace(key, value);
			}
		}
	}	
	
	if (this->method == "POST") {
		//������post����
		setBody(clientSock);
	}
	
	

}

// ����������
void Request::setBody(SOCKET clientSock)
{
	string length = this->head["Content-Length"]; // �����峤��
	string body; // ��ʱ����������-�ı��ļ�
	vector<char> body_binary;

	int len = atoi(length.c_str()); // string-ת-int
	if (len <= 0) return;

	string contentType = this->head["Content-Type"];
	cout << "contentType:" << contentType << endl;
	if (contentType.find("multipart/form-data") != -1) {
		get_body(clientSock, len, body_binary);
	}
	else {
		get_body(clientSock, len, body);
	}


	if (contentType.find("application/x-www-form-urlencoded") != -1) {
		parseForm(body);
	}
	else if (contentType.find("application/json") != -1) {
		parseJSON(body);
	}
	else if (contentType.find("text/xml") != -1) {
		parseXML(body);
	}
	else if (contentType.find("multipart/form-data") != -1) {
		parseFormData(body_binary);
	}
}

// // ����form�����ύ
void Request::parseForm(string &body)
{
	char* str = (char*)body.c_str();
	parseQuery(this->body_form, str); // ����form����������
}

// ����json��ʽ���ύ
void Request::parseJSON(string& body)
{
	this->body_json = JSON::parse(body);
	for (JSON::iterator it = this->body_json.begin(); it != this->body_json.end(); ++it) {
		std::cout << it.key() << " : " << it.value() << "\n";
	}
}

// ����XML��ʽ���ύ
void Request::parseXML(string& body)
{
	this->body_xml.Parse(body.c_str());
}

// ����FormData��ʽ���ύ
void Request::parseFormData(vector<char>& body)
{
	string contentType = this->head["Content-Type"];
	// ; �ָ� multipart/form-data; boundary=----WebKitFormBoundary5On24LeqJdcOBcBC
	size_t index = contentType.find("=");
	if (index == -1) return;
	
	

	string boundary = contentType.substr(index + 1);
	string start = "--" + boundary; // ÿһ�����ݶο�ʼλ��
	string end = start + "--"; // ���ݶν���λ��

	string buff; // �ı�����
	vector<char> part_buff;// �����ƻ���

	int startindex = 0; // ��ȡ����λ��
	unordered_map<string, string> partHead; // ���partͷ
	

	while (true)
	{
		buff.clear();
		startindex = get_line(body, buff, startindex);
		// ��ȡ����ʼ����
		if (buff == start) {
			buff.clear();
			startindex = parsePartHead(body, startindex, buff, partHead);
		}
		else if (buff == end) {// ��ȡ���˽�����־
			buff.clear();
			break;
		}
		
		if (buff.size() == 0) { // ��ȡ�������� { buff.size() == 0 ��ʾ������\r\n }
			buff.clear();

			// �ж����ı����� �ļ�
			if (partHead.count("filename") > 0) { // �������filename�ֶΣ�˵����һ���ļ�
				/* ��һ�ֶ�ȡ��ʽ */
				// startindex = parsePartBody_File(body, part_buff, startindex, start, end);
				/* �ڶ��ֶ�ȡ��ʽ */
				startindex = get_PartBody_File(body, part_buff, startindex);
				cout << "�ļ�:" << part_buff.size() << endl;
			}
			else {
				startindex = get_line(body, buff, startindex);
				cout << "�����ļ�:" << buff << endl;
			}
		}
	}


}






// ��ȡ������Ϣ
string Request::getRequest()
{
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
		string value = this->head.at("Accept-Encoding");
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
		printf("[%s] \n", buff);
		numchars = get_line(client, buff, sizeof(buff) - 1);
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
