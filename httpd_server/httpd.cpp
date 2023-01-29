#include "public_method.h"
#include <process.h>


int main(void) {
	unsigned short port = 8080;
	DWORD	threadId; // 线程ID
	HANDLE hThread; // 线程句柄
	/*初始化socket*/
	SOCKET serv_socket = startup(&port);

	printf("http服务器初始化，正在监听%d端口... \n", port);

	struct sockaddr_in client_addr; // 客户端信息
	int client_addr_len = sizeof(client_addr);

	// 循环等待socket
	while (true)
	{
		// 阻塞式等待用户通过浏览器发送请求
		SOCKET client_socket = accept(serv_socket, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_socket == -1) {
			error_die("accept");
		}

		// 创建一个新进程处理客户端请求
		hThread = (HANDLE)_beginthreadex(NULL, 0, accept_request, (void *)client_socket, 0, (unsigned *)&threadId);

	}
	closesocket(serv_socket);
	return 0;
}

