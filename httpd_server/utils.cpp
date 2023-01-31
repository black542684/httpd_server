#include "utils.h"
#include "GZipAssistant.h"

/*MIME类型*/
unordered_map<string, string> MIME = {
	// 文本
	{ "text", "text/plain" },
	{ "html", "text/html" },
	{ "css", "text/css" },
	{ "js", "text/javascript" },
	// 图像
	{ "gif", "image/gif" },
	{ "png", "image/png" },
	{ "jpg", "image/jpeg" },
	{ "bmp", "image/bmp" },
	{ "webp", "image/webp" },
	{ "ico", "image/x-icon" },
	// 音频
	{ "midi", "audio/midi" },
	{ "mp3", "audio/mpeg" },
	{ "ogg", "audio/ogg" },
	{ "wav", "audio/x-wav" },
	// 视频
	{ "mov", "video/quicktime" },
	{ "avi", "video/x-msvideo" },
	{ "movie", "video/x-sgi-movie" },
	{ "mpe", "video/mpeg" },
	{ "mpeg", "video/mpeg" },
	{ "html", "text/html" },
	{ "html", "text/html" },
};

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

//C++ 去字符串两边的空格
void delete_space(std::string& s)
{
	if (s.empty())
	{
		return;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
}

// 解析请求参数
void parseQuery(unordered_map<string, string>& query, char* queryStr) {
	char* outer_ptr = NULL;
	char* outer_ptr_query = NULL;

	char* chunk = strtok_s(queryStr, "&", &outer_ptr);
	while (chunk)
	{
		char* key = strtok_s(chunk, "=", &outer_ptr_query);
		char* value = strtok_s(NULL, "=", &outer_ptr_query);

		// 保存请求参数
		query[key] = value;
		chunk = strtok_s(NULL, "&", &outer_ptr);
	}
}


// 获取MIME类型
char* getContentType(const char* path) {
	// 返回指定的文件类型
	static char type[32] = { 0 };
	int len = strlen(path);
	int i = len;
	for (i; i > 0; i--) {
		if (path[i] == '.') {
			break;
		}
	}
	i++;

	int j = 0; // 保存类型的下标
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

	printf("文件类型是: %s \n", type);

	return type;
}

// 文件转成gzip
bool fileToGZIP(string filePath)
{
	FILE* fp = fopen(filePath.c_str(), "rb");
	size_t count; // 读取到的文件数量

	if (fp == NULL) return false;
	char buf[1024] = { 0 };
	string fileStr;
	
	printf("开始读取\n");
	while (true)
	{
		count = fread(buf, sizeof(char), sizeof(buf) - 1, fp);
		if (count == 0) {
			break;
		}
		buf[count] = '\0';

		fileStr.insert(fileStr.length(), buf, count);
	}
	printf("一共读取到：%lld字节\n", fileStr.length());
	fclose(fp);
	
	//压缩：
	GZipAssistant* gzip = GetGZipAssistant();
	const char* pSrc = fileStr.c_str();
	int nLenSrc = strlen(pSrc);

	int nLenCompress = nLenSrc * 2;
	char* pCompressed = new char[nLenCompress];
	memset(pCompressed, 0, nLenCompress);


	int nLencompressed = gzip->Compress(pSrc, nLenSrc, pCompressed, nLenCompress);
	printf("压缩之后的大小: %d \n", nLencompressed );
	if (nLencompressed <= 0)
	{
		printf("compress error.\n");
		return false;
	}

	// 保存之后的文件名
	string save_file = filePath + ".gz";
	// 保存压缩后的文件
	fp = fopen(save_file.c_str(), "wb");
	if (fp == NULL) return false;
	count = fwrite(pCompressed, sizeof(char), nLencompressed, fp);

	printf("一共写入: %lld \n" ,count);
	fclose(fp);
	fileStr.shrink_to_fit(); // 释放内存
	delete[] pCompressed;
	return true;
}

/*
* 页面未找到
*/
void not_found(SOCKET client) {
	// 向指定套接字，发送一个提示-未找到页面
	// 向指定套接字，发送一个响应头
	char buff[1024] = { 0 };

	strcpy(buff, "HTTP/1.1 404 Not Found\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Server: z-httpd/1.1\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Content-Type: %s; charset=utf-8\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);

	// 找到404页面
	FILE* resource = fopen("htdocs/404.html", "rb");

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