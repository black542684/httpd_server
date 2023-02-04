#include "utils.h"
#include "GZipAssistant.h"
#include <iostream>

/*MIME����*/
unordered_map<string, string> MIME = {
	// �ı�
	{ "text", "text/plain" },
	{ "html", "text/html" },
	{ "css", "text/css" },
	{ "js", "text/javascript" },
	// ͼ��
	{ "gif", "image/gif" },
	{ "png", "image/png" },
	{ "jpg", "image/jpeg" },
	{ "bmp", "image/bmp" },
	{ "webp", "image/webp" },
	{ "ico", "image/x-icon" },
	// ��Ƶ
	{ "midi", "audio/midi" },
	{ "mp3", "audio/mpeg" },
	{ "ogg", "audio/ogg" },
	{ "wav", "audio/x-wav" },
	// ��Ƶ
	{ "mov", "video/quicktime" },
	{ "avi", "video/x-msvideo" },
	{ "movie", "video/x-sgi-movie" },
	{ "mpe", "video/mpeg" },
	{ "mpeg", "video/mpeg" },
	{ "html", "text/html" },
	{ "html", "text/html" },
};

// ��ȡ���������Ϣ
void getNetworkError() {
	// ��ȡʧ�ܵ����
	int id = WSAGetLastError();
	switch (id)
	{
	case WSANOTINITIALISED: printf("��δִ�гɹ��� WSAStartup\n"); break;
	case WSASYSNOTREADY: printf("������ϵͳ�����á�\n"); break;
	case WSAHOST_NOT_FOUND: printf("�Ҳ���������\n"); break;
	case WSATRY_AGAIN: printf("�Ҳ�������Ȩ������\n"); break;
	case WSANO_RECOVERY: printf("���ǲ��ɻָ��Ĵ���\n"); break;
	case WSAEINPROGRESS: printf("�������ڽ����С�\n"); break;
	case WSANO_DATA: printf("��Ч���ƣ�û���������͵����ݼ�¼��\n"); break;
	case WSAEINTR: printf("�жϵĺ������á�\n"); break;
	case WSAEPROCLIM: printf("���̹��ࡣ\n");
	case WSAEFAULT: printf("��ַ����\n");
	case WSAENOTSOCK: printf("��Ч�׽����ϵ��׽��ֲ������ṩ���׽��־����Ч��\n");
	default: printf("δ֪���� id = %d\n", id); break;
	};
}

/*
* ��ȡ�����һ��
*/
int get_line(SOCKET sock, char* buff, int size) {
	if (buff == nullptr) {
		return 0;
	}

	char c = '\0'; // �����ȡ����һ���ַ�
	int i = 0; // �±�-  buff[i]


	while (i < size - 1 && c != '\n')
	{
		// �ӻ������ж�ȡһ���ַ�
		int n = recv(sock, &c, 1, 0);

		// recv����������ʵ��copy���ֽ���������0��ʾ��ȡ�����ַ�
		if (n > 0) {
			// ���ڻ��з���\r\n ���ָ�ʽ����Ҫ�ж�
			if (c == '\r') {
				// Ԥ�ȶ�ȡ���鿴��һ���ַ��ǲ���\n  ������ʹ�׽��ӽ��ն����е����ݼ���
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					n = recv(sock, &c, 1, 0); // ������ȡ����һ�� \n
				}
				else {
					c = '\n';
				}
			}
			if (c != '\r' && c != '\n') {
				buff[i++] = c;
			}
			
		}
		else {
			getNetworkError();
			c = '\n';
		}
	}

	buff[i] = '\0'; // ������
	return i;
}

/*
* ��vector�ж�ȡһ��
* @param body ������
* @param buff ��Ŷ�ȡ��������
* @param index ��ʼ��ȡ��λ��
* @return ����ȡ����λ��
*/
int get_line(vector<char>& body, string& buff, int index) {
	size_t len = body.size(); // ���鳤��
	char c = '\0'; // �����ȡ����һ���ַ�
	int i = index; // �±�-  buff[i]

	while (i < len && c != '\n') {
		c = body[i++];
		if (c == '\r' && body[i] == '\n') {
			i++;
			break;
		}
		if (c != '\n' && c != '\r') {
			buff.push_back(c);
		}
	}

	return i;
}

/*
* ��vector<char>�ж�ȡһ�У�������������һ��vector<char>��
* @param body ������
* @param buff ��Ŷ�ȡ��������
* @param index ��ʼ��ȡ��λ��
* @return ����ȡ����λ��
*/
int get_PartBody_File(vector<char>& body, vector<char>& buff, int index) {
	size_t len = body.size(); // ���鳤��
	char c = '\0'; // �����ȡ����һ���ַ�
	int i = index; // �±�-  buff[i]

	while (i < len) {
		c = body[i++];
		if (c == '\r' && body[i] == '\n') {
			i++;
			break;
		}
		buff.push_back(c);
	}

	return i;
}

/**
*  ����multipart/form-data  partͷ
* @param body ������
* @param index ��ǰ��ȡ����λ��
* @param buff ��ȡÿһ��
* @param partHead ���partͷ��map����
* @return ��ȡ����λ��
*/
int parsePartHead(vector<char>& body, int index, string& buff, unordered_map<string, string>& partHead) {
	int currentIndex = index;
	while (true)
	{
		// ��ȡPartͷ��
		buff.clear();
		currentIndex = get_line(body, buff, currentIndex);
		if (buff.size() == 0) break;
		/*
		* Content-Disposition: form-data; name="file"; filename="92cebf.jpg"
		*/
		// �����ȡ�����ļ�����
		char* outer_ptr = NULL;
		char* outer_ptr_query = NULL;

		char* queryStr = (char*)buff.c_str(); // ��Ҫ���ָ���ַ�����
		char* chunk = strtok_s(queryStr, ";", &outer_ptr);

		while (chunk)
		{
			string cun(chunk);
			char* tmp = NULL;
			string key;
			string value;

			if (cun.find("=") != -1) { // name="file"  �ԵȺŷָ�
				tmp = strtok_s(chunk, "=", &outer_ptr_query);
				key = tmp;
				delete_space(key);

				tmp = strtok_s(NULL, "=", &outer_ptr_query);
				value = tmp;
				delete_space(value); // ȥ���ַ������˿ո�
				delete_quotation(value); // ȥ���ַ�����������
			}
			else { // Content-Disposition: form-data  ��ð�ŷָ�
				tmp = strtok_s(chunk, ":", &outer_ptr_query);
				key = tmp;
				delete_space(key);

				tmp = strtok_s(NULL, ":", &outer_ptr_query);
				value = tmp;
				delete_space(value);
			}


			chunk = strtok_s(NULL, ";", &outer_ptr); // �ԣ��ָ�

			partHead.emplace(key, value); // ����partͷ
			cout << "cun:" << cun << endl;
			printf("key:%s\n", key.c_str());
			printf("value:%s\n", value.c_str());
		}
	}

	return currentIndex;
}

/**
* ����multipart/form-data  part �������ļ�����
* @param body ����Դ
* @param buff ������ݵ�
* @param index ��ʼ��ȡ��λ��
* @param startFlag boundary�ָ��־
* @param endFlag boundary�ָ��־
* @return ��ȡ������λ��
*/
int parsePartBody_File(vector<char>& body, vector<char>& buff, int index, string& startFlag, string& endFlag) {
	int startLen = startFlag.length();
	int endLen = endFlag.length();
	int len = body.size();
	int currentIndex = index;

	bool over = false; // �Ƿ��ȡ���ָ��־λ

	while (currentIndex < len)
	{

		// �ж��Ƿ��ȡ����
		if (currentIndex + startLen <= len) {
			// ��ȡvector
			auto first = body.begin() + currentIndex;
			auto last = body.begin() + currentIndex + startLen;
			string flag(first, last); // ��ȡ��������
			if (flag == startFlag) {
				over = true;
			}
		}

		if (currentIndex + endLen <= len) {
			// ��ȡvector
			auto first = body.begin() + currentIndex;
			auto last = body.begin() + currentIndex + endLen;
			string flag(first, last); // ��ȡ��������
			if (flag == startFlag) {
				over = true;
			}
		}

		if (over) { // ��ȡ����
			break;
		}
		buff.push_back(body[currentIndex++]); // �����ļ�
	}

	buff.pop_back(); // ȥ��\n
	buff.pop_back(); // ȥ��\r
	return currentIndex;
}

/**
* ��ȡ������-�ı�
*/
int get_body(SOCKET sock, int read_count,string& body) {
	char c = '\0'; // �����ȡ����һ���ַ�
	int i = 0; // �±�-  buff[i]
	int buffLen = read_count + 1;
	
	char* buff = new char[buffLen];

	while (i < read_count)
	{
		// �ӻ������ж�ȡһ���ַ�
		int n = recv(sock, &c, 1, 0);
		// recv����������ʵ��copy���ֽ���������0��ʾ��ȡ�����ַ�
		if (n > 0) {
			buff[i++] = c;
		}
		else {
			getNetworkError();
			c = '\n';
		}
	}

	buff[i] = '\0'; // ������
	body.append(buff);

	return i;
}

/**
* ��ȡ������-������
*/
int get_body(SOCKET sock, int read_count, vector<char>& body) {
	char c = '\0'; // �����ȡ����һ���ַ�
	int i = 0; // �±�-  buff[i]
	// int buffLen = read_count + 1;

	// char* buff = new char[buffLen];

	while (i < read_count)
	{
		// �ӻ������ж�ȡһ���ַ�
		int n = recv(sock, &c, 1, 0);
		// recv����������ʵ��copy���ֽ���������0��ʾ��ȡ�����ַ�
		if (n > 0) {
			// buff[i++] = c;
			i++;
			body.push_back(c);
		}
		else {
			getNetworkError();
			c = '\n';
		}
	}

	// buff[i] = '\0'; // ������
	
	// body.append(buff);

	return i;
}

// �����ո�
void trimStart(char* str, int size, int* index) {
	while (*index < size)
	{
		if (isspace(str[*index])) {
			*index += 1;
		}
		else {
			break;
		}
	}
}

//ȥ�ַ������ߵĿո�
void delete_space(string& s)
{
	if (s.empty())
	{
		return;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
}

// ȥ��string ��������
void delete_quotation(string& s) {
	if (s.empty())
	{
		return;
	}
	s.erase(0, s.find_first_not_of("\""));
	s.erase(s.find_last_not_of("\"") + 1);
}

// �����������
void parseQuery(unordered_map<string, string>& query, char* queryStr) {
	char* outer_ptr = NULL;
	char* outer_ptr_query = NULL;

	char* chunk = strtok_s(queryStr, "&", &outer_ptr);
	while (chunk)
	{
		char* key = strtok_s(chunk, "=", &outer_ptr_query);
		char* value = strtok_s(NULL, "=", &outer_ptr_query);

		// �����������
		query[key] = value;
		chunk = strtok_s(NULL, "&", &outer_ptr);
	}
}

// ��ȡMIME����
char* getContentType(const char* path) {
	
	// ����ָ�����ļ�����
	static char type[32] = { 0 };
	int len = strlen(path);
	int i = len;
	for (i; i > 0; i--) {
		if (path[i] == '.') {
			break;
		}
	}
	i++;

	int j = 0; // �������͵��±�
	for (i, j; i < len && j < sizeof(type) && path[i] != '\0'; i++, j++) {
		type[j] = path[i];
	}

	type[j] = '\0';

	string value;
	try {
		value = MIME.at(type);
	}
	catch (const out_of_range& e) {
		printf("MIME ��ȡʧ��: %s \n", e.what());
		value = "text/plain";
	}

	strcpy(type, value.c_str());

	//printf("�ļ�������: %s \n", type);

	return type;
}

// �ļ�ת��gzip
bool fileToGZIP(string filePath)
{
	FILE* fp = fopen(filePath.c_str(), "rb");
	size_t count; // ��ȡ�����ļ�����

	if (fp == NULL) return false;
	char buf[1024] = { 0 };
	string fileStr;
	
	printf("��ʼ��ȡ\n");
	while (true)
	{
		count = fread(buf, sizeof(char), sizeof(buf) - 1, fp);
		if (count == 0) {
			break;
		}
		buf[count] = '\0';

		fileStr.insert(fileStr.length(), buf, count);
	}
	printf("һ����ȡ����%lld�ֽ�\n", fileStr.length());
	fclose(fp);
	
	//ѹ����
	GZipAssistant* gzip = GetGZipAssistant();
	const char* pSrc = fileStr.c_str();
	int nLenSrc = strlen(pSrc);

	int nLenCompress = nLenSrc * 2;
	char* pCompressed = new char[nLenCompress];
	memset(pCompressed, 0, nLenCompress);


	int nLencompressed = gzip->Compress(pSrc, nLenSrc, pCompressed, nLenCompress);
	printf("ѹ��֮��Ĵ�С: %d \n", nLencompressed );
	if (nLencompressed <= 0)
	{
		printf("compress error.\n");
		return false;
	}

	// ����֮����ļ���
	string save_file = filePath + ".gz";
	// ����ѹ������ļ�
	fp = fopen(save_file.c_str(), "wb");
	if (fp == NULL) return false;
	count = fwrite(pCompressed, sizeof(char), nLencompressed, fp);

	printf("һ��д��: %lld \n" ,count);
	fclose(fp);
	fileStr.shrink_to_fit(); // �ͷ��ڴ�
	delete[] pCompressed;
	return true;
}


// ҳ��δ�ҵ�
void not_found(SOCKET client) {
	// ��ָ���׽��֣�����һ����ʾ-δ�ҵ�ҳ��
	// ��ָ���׽��֣�����һ����Ӧͷ
	char buff[1024] = { 0 };

	strcpy(buff, "HTTP/1.1 404 Not Found\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Server: z-httpd/1.1\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Content-Type: %s; charset=utf-8\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);

	// �ҵ�404ҳ��
	FILE* resource = fopen("htdocs/404.html", "rb");

	while (true)
	{
		int ret = fread(buff, sizeof(char), sizeof(buff), resource);
		// û�ж�ȡ��
		if (ret <= 0) {
			break;
		}
		send(client, buff, ret, 0);
	}

}

// ��ӡmap
void printMap(unordered_map<string, string>& map) {
	for (auto it = map.begin(); it != map.end(); it++) {
		cout << it->first << ":" << it->second << endl;
	}
}