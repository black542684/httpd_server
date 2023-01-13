#include "utils.h"
/*
* 读取请求的一行
*/
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
			case WSANOTINITIALISED: printf("尚未执行成功的 WSAStartup\n"); break;
			case WSASYSNOTREADY: printf("网络子系统不可用。\n"); break;
			case WSAHOST_NOT_FOUND: printf("找不到主机。\n"); break;
			case WSATRY_AGAIN: printf("找不到非授权主机。\n"); break;
			case WSANO_RECOVERY: printf("这是不可恢复的错误。\n"); break;
			case WSAEINPROGRESS: printf("操作正在进行中。\n"); break;
			case WSANO_DATA: printf("有效名称，没有请求类型的数据记录。\n"); break;
			case WSAEINTR: printf("中断的函数调用。\n"); break;
			case WSAEPROCLIM: printf("进程过多。\n");
			case WSAEFAULT: printf("地址错误。\n");
			case WSAENOTSOCK: printf("无效套接字上的套接字操作。提供的套接字句柄无效。\n");
			default: printf("未知错误 id = %d\n", id); break;
			};
			printf("收到错误.\n");
			c = '\n';
		}
	}

	buff[i] = '\0'; // 结束符
	return i;
}

/*
* 跳过空格
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