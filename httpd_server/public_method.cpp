#include "public_method.h"
#include "utils.h"
#include "request.h"

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

void error_die(const char* str) {
	perror(str);
	exit(1);
}



/*
* ��������֧����������
*/
void unimplement(SOCKET client) {
	// ��ָ���׽��֣�����һ����ʾ-��û��ʵ�ֵĴ���ҳ��
	char buff[1024] = { 0 };

	strcpy(buff, "HTTP/1.1 502 Bad Gateway\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Server: z-httpd/1.1\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Content-Type: %s; charset=utf-8\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);

	// �ҵ�502ҳ��
	FILE* resource = fopen("htdocs/502.html", "rb");

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

/*
* ��ȡMIME����
*/
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
		std::cerr << "unexpected exception: " << e.what() << std::endl;
		value = "text/plain";
	}

	strcpy(type, value.c_str());

	printf("�ļ�������: %s \n", type);

	return type;
}

/*
* ������Ӧͷ
*/
void headers(SOCKET client, const char* type) {

	// ��ָ���׽��֣�����һ����Ӧͷ
	char buff[1024] = { 0 };

	strcpy(buff, "HTTP/1.1 200 OK\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Server: z-httpd/1.1\r\n");
	send(client, buff, strlen(buff), 0);

	// html �ļ�
	if (strcmp(type, "text/html") == 0) {
		sprintf(buff, "Content-Type: %s; charset=utf-8\r\n", type);
	}
	else {
		sprintf(buff, "Content-Type: %s;\r\n", type);
	}
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);
}

/*
* ������Դ
*/
void cat(SOCKET client, FILE* resource) {
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
		send(client, buff, ret, 0);
		count += ret;
	}

	printf("һ�������������%lld���ֽ�\n", count);


}

/*
* ��Ͷ˷���һ��ҳ��
*/
void server_file(SOCKET client, const char* path) {
	// ��ͻ��˷����ļ�
	int numchars = 1;
	char buff[1024] = { 0 };

	// �������ʣ�����ݶ�ȡ���
	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(client, buff, sizeof(buff));
		PRINTF(buff);
	}

	FILE* resource = fopen(path, "rb");

	// �ļ�������
	if (resource == NULL) {
		not_found(client);
	}
	else {
		// ������Ӧͷ
		headers(client, getContentType(path));

		// �����������Դ��Ϣ
		cat(client, resource);

		printf("��Դ������ϡ�\n\n\n");

		// �ر��ļ�
		fclose(resource);
	}



}

/*
* ����ͨ�ų�ʼ��
*/
SOCKET startup(unsigned short* port) {
	// 1. ����ͨ�ų�ʼ��
	WSADATA data;
	int result = WSAStartup(
		MAKEWORD(1, 1),
		&data
	);
	if (result) { // ����0��ʾ�ɹ�����Ϊ0��ʾʧ��
		error_die("WSAStartup");
	}

	// 2.�����׽���
	SOCKET server_socket = socket(
		PF_INET, // �׽�������-IPV4
		SOCK_STREAM, // ������-TCP
		IPPROTO_TCP  // Э��
	);

	if (server_socket == -1) {
		// ��ӡ������ʾ����������
		error_die("�׽���");
	}

	// 3.���ö˿ڿɸ���
	int opt = 1;
	result = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (result == -1) {
		error_die("setsockopt");
	}

	// 4.���÷������˵������ַ
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;// �׽�������
	server_addr.sin_port = htons(*port); // �˿�
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip��ַ-�����ַ



	// ���׽���
	result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (result < 0) {
		error_die("bind");
	}

	// ��̬����һ���˿�
	if (*port == 0) {
		int nameLen = sizeof(server_addr);
		result = getsockname(server_socket, (SOCKADDR*)&server_addr, &nameLen);
		if (result < 0) {
			error_die("getsockname");
		}

		*port = server_addr.sin_port;
	}

	// ������������
	if (listen(server_socket, 5) < 0) {
		error_die("listen");
	}

	return server_socket;
}

/*
* �����û�������߳�
*/
unsigned WINAPI accept_request(void* arg) {

	
	SOCKET client = (SOCKET)arg; // �ͻ����׽���

	Request request(client);

	string method = request.getMethods();
	
	PRINTF(method.c_str());

	// �ж��Ƿ��� GET ���� POST ����
	if (method == "GET" && method ==  "POST") {
		// �����������һ������ҳ��
		unimplement(client);
		return 0;
	}


	string url = request.getUrl();

	PRINTF(url.c_str());

	// �����ļ���·��
	// www.baidu.com
	// 127.0.0.1/a/b/c
	char path[512] = { 0 };
	sprintf(path, "htdocs%s", url.c_str());  // �ַ���ƴ��-·��ƴ��

	// �ж�url �ǲ��� /
	if (path[strlen(path) - 1] == '/') {
		// ����� / �򷵻� index.html
		strcat(path, "index.html");
	}
	PRINTF(path);

	// �ж�path ·��������-�ļ�or�ļ���
	struct stat status;

	// û�з����ļ������ļ���
	if (stat(path, &status) == -1) {
		// �������ʣ�����ݶ�ȡ���
		request.release();
		not_found(client);
	}
	else {
		// �ж��ļ�����
		if ((status.st_mode & S_IFMT) == S_IFDIR) {
			// ������ļ���-��ƴ�� index.html
			strcat(path, "index.html");
		}
		// �������������ҳ
		server_file(client, path);

	}


	//�ر�socket
	closesocket(client);


	return 0;
}
