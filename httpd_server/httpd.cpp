#include "public_method.h"
#include <process.h>


int main(void) {
	unsigned short port = 8080;
	DWORD	threadId; // �߳�ID
	HANDLE hThread; // �߳̾��
	/*��ʼ��socket*/
	SOCKET serv_socket = startup(&port);

	printf("http��������ʼ�������ڼ���%d�˿�... \n", port);

	struct sockaddr_in client_addr; // �ͻ�����Ϣ
	int client_addr_len = sizeof(client_addr);

	// ѭ���ȴ�socket
	while (true)
	{
		// ����ʽ�ȴ��û�ͨ���������������
		SOCKET client_socket = accept(serv_socket, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_socket == -1) {
			error_die("accept");
		}

		// ����һ���½��̴���ͻ�������
		hThread = (HANDLE)_beginthreadex(NULL, 0, accept_request, (void *)client_socket, 0, (unsigned *)&threadId);

	}
	closesocket(serv_socket);
	return 0;
}

