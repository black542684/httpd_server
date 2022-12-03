#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#define PRINTF(str) printf("[%s - %d] "#str" = %s \n", __func__, __LINE__, str);

/**
* 打印错误信息并推出程序
* @param str 错误提示
*/
void error_die(const char* str);
/**
* 网络通信初始化
* @param port 
* @return socket
*/
SOCKET startup(unsigned short* port);

// 读取请求的一行
int get_line(SOCKET sock, char* buff, int size);

// 处理错误请求
void unimplement(SOCKET client);

// 没有找到页面
void not_found(SOCKET client);

// 获取资源类型
char* getContentType(const char* path);

// 发送响应头
void headers(SOCKET client, const char* type);

// 发送请求的资源信息
void cat(SOCKET client, FILE* resource);

// 向客户端发送一个页面
void server_file(SOCKET client, const char* path);

// 处理用户请求的线程
DWORD WINAPI accept_request(LPVOID arg);

// 跳过空格
void trimStart(char* str, int size, int* index);

int main(void) {
	unsigned short port = 8080;
	SOCKET socket = startup(&port);
	printf("http服务器初始化，正在监听%d端口... \n", port);

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	// 循环等待socket
	while (true)
	{
		// 阻塞式等待用户通过浏览器发送请求
		SOCKET client_socket = accept(socket, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_socket == -1) {
			error_die("accept");
		}

		DWORD	threadId; // 线程ID
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
	// 向指定套接字，发送一个提示-还没有实现的错误页面

}

void not_found(SOCKET client) {
	// 向指定套接字，发送一个提示-未找到页面
}

char* getContentType(const char* path) {
	// 返回指定的文件类型
	static char type[32] = {0};
	int len = strlen(path);
	int i = len;
	for (i; i > 0; i--) {
		if (path[i] == '.') {
			break;
		}
	}

	int j = 0; // 保存类型的下标
	for (i, j; i < len && j < sizeof(type) && path[i] != '\0'; i++, j++) {
		type[j] = path[i];
	}

	type[j] = '\0';


	

	if (strcmp(type, ".jpg") == 0) strcpy(type, "image/jpeg");
	if (strcmp(type, ".html") == 0) strcpy(type, "text/html");
	printf("文件类型是 %s \n", type);

	return type;
}

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

void cat(SOCKET client, FILE* resource) {
	// 向指定套接字，发送响应体

	char buff[4096] = { 0 };
	int count = 0; // 记录读取到的字节

	while (true)
	{	
		int ret = fread(buff, sizeof(char), sizeof(buff), resource);
		// 没有读取到
		if (ret <= 0) {
			break;
		}
		send(client, buff, ret, 0);
		count += ret;
	}
	
	printf("一共向浏览器发送%d个字节\n", count);


}

void server_file(SOCKET client, const char* path) {
	// 向客户端发送文件
	int numchars = 1;
	char buff[1024] = {0};

	// 请求包的剩余数据读取完毕
	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(client, buff, sizeof(buff));
		PRINTF(buff);
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

		printf("资源发送完毕。\n");

		// 关闭文件
		fclose(resource);
	}

	

}

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
		PF_INET, // 套接字类型
		SOCK_STREAM, // 数据流
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
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;// 套接字类型
	server_addr.sin_port = htons(*port); // 端口
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip地址



	// 绑定套接字
	result = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (result < 0) {
		error_die("bind");
	}

	// 动态分配一个端口
	if (*port == 0) {
		int nameLen = sizeof(server_addr);
		result = getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen);
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

int get_line(SOCKET sock, char* buff, int size) {

	char c = '\0'; // 保存读取到的一个字符
	int i = 0; // 下标-  buff[i]


	while (i < size - 1 && c != '\n')
	{
		// 从缓冲区中读取一个字符
		int n = recv(sock, &c, 1, 0);
		
		// recv函数返回其实际copy的字节数，大于0表示读取到了字符
		if (n > 0) {
			// 由于换行符是\r\n 这种格式，需要判断
			if (c == '\r') {
				// 查看下一个字符是不是\n
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					recv(sock, &c, 1, 0); // 正常读取
				}
				else {
					c = '\n';
				}
			}
			buff[i++] = c;
		}
		else {
			// 读取失败的情况
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
				case WSAENOTSOCK: printf("无效套接字上的套接字操作。提供的套接字句柄无效。\n");
				default: printf("unknown error id = %d\n", id); break;
			};
			printf("receive error.\n");
			c = '\n';
		}
	}

	buff[i] = '\0'; // 结束符
	return i;
}

DWORD WINAPI accept_request(LPVOID arg) {
	char buff[1024] = {0}; // 1K

	SOCKET client = (SOCKET)arg; // 客户端套接字

	// 读取一行数据
	int numchars = get_line(client, buff, sizeof(buff));
	PRINTF(buff);

	int j = 0; // buff下标
	int i = 0; // method下标
	char method[255] = {0}; // 请求方法 GET or POST

	trimStart(buff, sizeof(buff), &j); // 跳过空格

	while (!isspace(buff[j]) && i < sizeof(method) - 1)
	{
		method[i++] = buff[j++];
	}

	method[i] = '\0';
	PRINTF(method);

	// 判断是否是 GET 或者 POST 请求
	if (_stricmp(method, "GET") && _stricmp(method, "POST")) {
		// 向浏览器返回一个错误页面
		unimplement(client);
		return 0;
	}

	
	// 解析资源文件路径
	char url[255] = {0}; // 存放完整资源路径
	trimStart(buff, sizeof(buff), &j); // 跳过空格
	
	i = 0; // 重置
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

	// 访问文件的路径

	// www.baidu.com
	// 127.0.0.1/a/b/c
	char path[512] = {0};
	sprintf(path, "htdocs%s", url);  // 字符串拼接-路径拼接

	// 判断url 是不是 /
	if (path[strlen(path) - 1] == '/') {
		// 如果是 / 则返回 index.html
		strcat(path, "index.html");
	}
	PRINTF(path);

	// 判断path 路径的属性-文件or文件夹
	struct stat status;

	// 没有发现文件或者文件夹
	if (stat(path, &status) == -1) {
		// 请求包的剩余数据读取完毕
		while (numchars > 0 && strcmp(buff, "\n"))
		{
			numchars = get_line(client, buff, sizeof(buff));
		}
		
		not_found(client);
	}
	else {
		// 判断文件类型
		if ((status.st_mode & S_IFMT) == S_IFDIR) {
			// 如果是文件夹-则拼接 index.html
			strcat(path, "index.html");
		}
		// 向浏览器返回网页
		server_file(client, path);

	}
	

	//关闭socket
	closesocket(client);


	return 0;
}