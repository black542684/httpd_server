#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#define PRINTF(str) printf("[%s - %d] "#str" = %s \n", __func__, __LINE__, str);

/**
* ��ӡ������Ϣ���Ƴ�����
* @param str ������ʾ
*/
void error_die(const char* str);
/**
* ����ͨ�ų�ʼ��
* @param port 
* @return socket
*/
SOCKET startup(unsigned short* port);

// ��ȡ�����һ��
int get_line(SOCKET sock, char* buff, int size);

// �����������
void unimplement(SOCKET client);

// û���ҵ�ҳ��
void not_found(SOCKET client);

// ��ȡ��Դ����
char* getContentType(const char* path);

// ������Ӧͷ
void headers(SOCKET client, const char* type);

// �����������Դ��Ϣ
void cat(SOCKET client, FILE* resource);

// ��ͻ��˷���һ��ҳ��
void server_file(SOCKET client, const char* path);

// �����û�������߳�
DWORD WINAPI accept_request(LPVOID arg);

// �����ո�
void trimStart(char* str, int size, int* index);

int main(void) {
	unsigned short port = 8080;
	SOCKET socket = startup(&port);
	printf("http��������ʼ�������ڼ���%d�˿�... \n", port);

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	// ѭ���ȴ�socket
	while (true)
	{
		// ����ʽ�ȴ��û�ͨ���������������
		SOCKET client_socket = accept(socket, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_socket == -1) {
			error_die("accept");
		}

		DWORD	threadId; // �߳�ID
		CreateThread(0, 0, accept_request, (void *)client_socket, 0, &threadId);

	}


	closesocket(socket);
	return 0;
}

void error_die(const char* str) {
	perror(str);
	exit(1);
}

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

void unimplement(SOCKET client) {
	// ��ָ���׽��֣�����һ����ʾ-��û��ʵ�ֵĴ���ҳ��

}

void not_found(SOCKET client) {
	// ��ָ���׽��֣�����һ����ʾ-δ�ҵ�ҳ��
}

char* getContentType(const char* path) {
	// ����ָ�����ļ�����
	static char type[32] = {0};
	int len = strlen(path);
	int i = len;
	for (i; i > 0; i--) {
		if (path[i] == '.') {
			break;
		}
	}

	int j = 0; // �������͵��±�
	for (i, j; i < len && j < sizeof(type) && path[i] != '\0'; i++, j++) {
		type[j] = path[i];
	}

	type[j] = '\0';


	

	if (strcmp(type, ".jpg") == 0) strcpy(type, "image/jpeg");
	if (strcmp(type, ".html") == 0) strcpy(type, "text/html");
	printf("�ļ������� %s \n", type);

	return type;
}

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

void cat(SOCKET client, FILE* resource) {
	// ��ָ���׽��֣�������Ӧ��

	char buff[4096] = { 0 };
	int count = 0; // ��¼��ȡ�����ֽ�

	while (true)
	{	
		int ret = fread(buff, sizeof(char), sizeof(buff), resource);
		// û�ж�ȡ��
		if (ret <= 0) {
			break;
		}
		send(client, buff, ret, 0);
		count += ret;
	}
	
	printf("һ�������������%d���ֽ�\n", count);


}

void server_file(SOCKET client, const char* path) {
	// ��ͻ��˷����ļ�
	int numchars = 1;
	char buff[1024] = {0};

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

		printf("��Դ������ϡ�\n");

		// �ر��ļ�
		fclose(resource);
	}

	

}

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
		PF_INET, // �׽�������
		SOCK_STREAM, // ������
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
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;// �׽�������
	server_addr.sin_port = htons(*port); // �˿�
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip��ַ



	// ���׽���
	result = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (result < 0) {
		error_die("bind");
	}

	// ��̬����һ���˿�
	if (*port == 0) {
		int nameLen = sizeof(server_addr);
		result = getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen);
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
				case WSANOTINITIALISED: printf("not initialized\n"); break;
				case WSASYSNOTREADY: printf("sub sys not ready\n"); break;
				case WSAHOST_NOT_FOUND: printf("name server not found\n"); break;
				case WSATRY_AGAIN: printf("server fail\n"); break;
				case WSANO_RECOVERY: printf("no recovery\n"); break;
				case WSAEINPROGRESS: printf("socket blocked by other prog\n"); break;
				case WSANO_DATA: printf("no data record\n"); break;
				case WSAEINTR: printf("blocking call canciled\n"); break;
				case WSAEPROCLIM: printf("limit exceeded\n");
				case WSAEFAULT: printf("lpWSAData in startup not valid\n");
				case WSAENOTSOCK: printf("��Ч�׽����ϵ��׽��ֲ������ṩ���׽��־����Ч��\n");
				default: printf("unknown error id = %d\n", id); break;
			};
			printf("receive error.\n");
			c = '\n';
		}
	}

	buff[i] = '\0'; // ������
	return i;
}

DWORD WINAPI accept_request(LPVOID arg) {
	char buff[1024] = {0}; // 1K

	SOCKET client = (SOCKET)arg; // �ͻ����׽���

	// ��ȡһ������
	int numchars = get_line(client, buff, sizeof(buff));
	PRINTF(buff);

	int j = 0; // buff�±�
	int i = 0; // method�±�
	char method[255] = {0}; // ���󷽷� GET or POST

	trimStart(buff, sizeof(buff), &j); // �����ո�

	while (!isspace(buff[j]) && i < sizeof(method) - 1)
	{
		method[i++] = buff[j++];
	}

	method[i] = '\0';
	PRINTF(method);

	// �ж��Ƿ��� GET ���� POST ����
	if (_stricmp(method, "GET") && _stricmp(method, "POST")) {
		// �����������һ������ҳ��
		unimplement(client);
		return 0;
	}

	
	// ������Դ�ļ�·��
	char url[255] = {0}; // ���������Դ·��
	trimStart(buff, sizeof(buff), &j); // �����ո�
	
	i = 0; // ����
	while (
		!isspace(buff[j]) && 
		i < sizeof(url) - 1  && 
		j < sizeof(buff)
	)
	{
		url[i++] = buff[j++];
	}
	url[i] = '\0';
	PRINTF(url);

	// �����ļ���·��

	// www.baidu.com
	// 127.0.0.1/a/b/c
	char path[512] = {0};
	sprintf(path, "htdocs%s", url);  // �ַ���ƴ��-·��ƴ��

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
		while (numchars > 0 && strcmp(buff, "\n"))
		{
			numchars = get_line(client, buff, sizeof(buff));
		}
		
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