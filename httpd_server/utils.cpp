#include "utils.h"
#include "GZipAssistant.h"

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

/*
* ��ȡ�����һ��
*/
int get_line(SOCKET sock, char* buff, int size) {

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
				// �鿴��һ���ַ��ǲ���\n
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					recv(sock, &c, 1, 0); // ������ȡ
				}
				else {
					c = '\n';
				}
			}
			buff[i++] = c;
		}
		else {
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
			printf("�յ�����.\n");
			c = '\n';
		}
	}

	buff[i] = '\0'; // ������
	return i;
}

/*
* �����ո�
*/
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

//C++ ȥ�ַ������ߵĿո�
void delete_space(std::string& s)
{
	if (s.empty())
	{
		return;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
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
		printf("unexpected exception: %s \n", e.what());
		value = "text/plain";
	}

	strcpy(type, value.c_str());

	printf("�ļ�������: %s \n", type);

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

/*
* ҳ��δ�ҵ�
*/
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