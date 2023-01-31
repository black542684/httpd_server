#include "public_method.h"
#include "utils.h"



void error_die(const char* str) {
	perror(str);
	exit(1);
}

/*
* 服务器不支持请求类型
*/
void unimplement(SOCKET client) {
	// 向指定套接字，发送一个提示-还没有实现的错误页面
	char buff[1024] = { 0 };

	strcpy(buff, "HTTP/1.1 502 Bad Gateway\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Server: z-httpd/1.1\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Content-Type: %s; charset=utf-8\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);

	// 找到502页面
	FILE* resource = fopen("htdocs/502.html", "rb");

	while (true)
	{
		int ret = fread(buff, sizeof(char), sizeof(buff), resource);
		// 没有读取到
		if (ret <= 0) {
			break;
		}
		send(client, buff, ret, 0);
	}
}


/*
* 网络通信初始化
*/
SOCKET startup(unsigned short* port) {
	// 1. 网络通信初始化
	WSADATA data;
	int result = WSAStartup(
		MAKEWORD(1, 1),
		&data
	);
	if (result) { // 返回0表示成功，不为0表示失败
		error_die("WSAStartup");
	}

	// 2.创建套接字
	SOCKET server_socket = socket(
		PF_INET, // 套接字类型-IPV4
		SOCK_STREAM, // 数据流-TCP
		IPPROTO_TCP  // 协议
	);

	if (server_socket == -1) {
		// 打印错误提示并结束程序
		error_die("套接字");
	}

	// 3.设置端口可复用
	int opt = 1;
	result = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (result == -1) {
		error_die("setsockopt");
	}

	// 4.配置服务器端的网络地址
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr)); // 初始化为0
	server_addr.sin_family = PF_INET;// 套接字类型
	server_addr.sin_port = htons(*port); // 端口-小端转大端
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip地址-127.0.0.1  小端转大端



	// 绑定套接字
	result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (result < 0) {
		error_die("bind");
	}

	// 动态分配一个端口
	if (*port == 0) {
		int nameLen = sizeof(server_addr);// 地址缓冲区长度
		// 获取一个套接口的本地名字
		result = getsockname(
			server_socket, // 标识一个已捆绑套接口的描述符号
			(SOCKADDR*)&server_addr, // 接收套接口的地址
			&nameLen // 地址缓冲区长度-字节
		);

		if (result < 0) {
			error_die("getsockname");
		}
		*port = server_addr.sin_port;
	}

	// 创建监听队列
	if (listen(server_socket, 5) < 0) {
		error_die("listen");
	}

	return server_socket;
}

/*
* 处理用户请求的线程
*/
unsigned WINAPI accept_request(void* arg) {

	SOCKET client = (SOCKET)arg; // 客户端套接字

	Request request(client);// 请求
	Response response(request, "htdocs"); // 响应


	cout << "methods " << request.getMethods() << endl;

	// 判断是否是 GET 或者 POST 请求
	if (request.getMethods() != "GET" && request.getMethods() !=  "POST") {
		// 向浏览器返回一个错误页面
		unimplement(client);
		closesocket(client);
		return 0;
	}

	PRINTF(request.getUrl().c_str());

	response.send();

	//关闭socket
	closesocket(client);
	return 0;
}
