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
* 发送响应头
*/
void headers(SOCKET client, const char* type) {

	// 向指定套接字，发送一个响应头
	char buff[1024] = { 0 };

	strcpy(buff, "HTTP/1.1 200 OK\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Server: z-httpd/1.1\r\n");
	send(client, buff, strlen(buff), 0);

	// html 文件
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
* 发送资源
*/
void cat(SOCKET client, FILE* resource) {
	// 向指定套接字，发送响应体

	char buff[4096] = { 0 };
	size_t count = 0; // 记录读取到的字节

	while (true)
	{
		size_t ret = fread(buff, sizeof(char), sizeof(buff), resource);
		// 没有读取到
		if (ret <= 0) {
			break;
		}
		send(client, buff, ret, 0);
		count += ret;
	}

	printf("一共向浏览器发送%lld个字节\n", count);


}

/*
* 向客端发送一个页面
*/
void server_file(SOCKET client, const char* path, Request& request) {
	// 向客户端发送文件
	int numchars = 1;
	char buff[1024] = { 0 };

	if (request.isgzip()) {
		printf("支持gzip \n");
	}

	FILE* resource = fopen(path, "rb");

	// 文件不存在
	if (resource == NULL) {
		not_found(client);
	}
	else {
		// 发送响应头
		headers(client, getContentType(path));

		// 发送请求的资源信息
		cat(client, resource);

		printf("资源发送完毕。\n\n\n");

		// 关闭文件
		fclose(resource);
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
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;// 套接字类型
	server_addr.sin_port = htons(*port); // 端口
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip地址-任意地址



	// 绑定套接字
	result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (result < 0) {
		error_die("bind");
	}

	// 动态分配一个端口
	if (*port == 0) {
		int nameLen = sizeof(server_addr);
		result = getsockname(server_socket, (SOCKADDR*)&server_addr, &nameLen);
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

	Request request(client);

	string method = request.getMethods();
	
	cout << "methods " << method << endl;

	// 判断是否是 GET 或者 POST 请求
	if (method == "GET" && method ==  "POST") {
		// 向浏览器返回一个错误页面
		unimplement(client);
		closesocket(client);
		return 0;
	}


	string url = request.getUrl();


	PRINTF(url.c_str());

	Response response(request, "htdocs");

	response.send();

	//// 访问文件的路径
	//char path[512] = { 0 };
	//sprintf(path, "htdocs%s", url.c_str());  // 字符串拼接-路径拼接

	//// 判断url 是不是 /
	//if (path[strlen(path) - 1] == '/') {
	//	// 如果是 / 则返回 index.html
	//	strcat(path, "index.html");
	//}
	//PRINTF(path);

	//// 判断path 路径的属性-文件or文件夹
	//struct stat status;

	//// 没有发现文件或者文件夹
	//if (stat(path, &status) == -1) {
	//	not_found(client);
	//}
	//else {
	//	// 判断文件类型
	//	if ((status.st_mode & S_IFMT) == S_IFDIR) {
	//		// 如果是文件夹-则拼接 index.html
	//		strcat(path, "index.html");
	//	}
	//	// 向浏览器返回网页
	//	server_file(client, path, request);

	//}


	//关闭socket
	closesocket(client);


	return 0;
}
