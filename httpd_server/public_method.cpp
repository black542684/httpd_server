#include "public_method.h"
#include "utils.h"



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
	memset(&server_addr, 0, sizeof(server_addr)); // ��ʼ��Ϊ0
	server_addr.sin_family = PF_INET;// �׽�������
	server_addr.sin_port = htons(*port); // �˿�-С��ת���
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip��ַ-127.0.0.1  С��ת���



	// ���׽���
	result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (result < 0) {
		error_die("bind");
	}

	// ��̬����һ���˿�
	if (*port == 0) {
		int nameLen = sizeof(server_addr);// ��ַ����������
		// ��ȡһ���׽ӿڵı�������
		result = getsockname(
			server_socket, // ��ʶһ���������׽ӿڵ���������
			(SOCKADDR*)&server_addr, // �����׽ӿڵĵ�ַ
			&nameLen // ��ַ����������-�ֽ�
		);

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

	Request request(client);// ����
	Response response(request, "htdocs"); // ��Ӧ


	cout << "methods " << request.getMethods() << endl;

	// �ж��Ƿ��� GET ���� POST ����
	if (request.getMethods() != "GET" && request.getMethods() !=  "POST") {
		// �����������һ������ҳ��
		unimplement(client);
		closesocket(client);
		return 0;
	}

	PRINTF(request.getUrl().c_str());

	response.send();

	//�ر�socket
	closesocket(client);
	return 0;
}
