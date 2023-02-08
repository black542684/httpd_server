#include "utils.h"
#include "GZipAssistant.h"
#include "Algorithm.hpp"
#include <iostream>

/*MIME类型*/
unordered_map<string, string> MIME = {
	// 文本
	{ "text", "text/plain" },
	{ "html", "text/html" },
	{ "css", "text/css" },
	{ "js", "text/javascript" },
	{ "json", "application/json" },
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

// 获取网络错误消息
void getNetworkError() {
	// 读取失败的情况
	int id = WSAGetLastError();
	switch (id)
	{
	case WSANOTINITIALISED: printf(u8"尚未执行成功的 WSAStartup\n"); break;
	case WSASYSNOTREADY: printf(u8"网络子系统不可用。\n"); break;
	case WSAHOST_NOT_FOUND: printf(u8"找不到主机。\n"); break;
	case WSATRY_AGAIN: printf(u8"找不到非授权主机。\n"); break;
	case WSANO_RECOVERY: printf(u8"这是不可恢复的错误。\n"); break;
	case WSAEINPROGRESS: printf(u8"操作正在进行中。\n"); break;
	case WSANO_DATA: printf(u8"有效名称，没有请求类型的数据记录。\n"); break;
	case WSAEINTR: printf(u8"中断的函数调用。\n"); break;
	case WSAEPROCLIM: printf(u8"进程过多。\n");
	case WSAEFAULT: printf(u8"地址错误。\n");
	case WSAENOTSOCK: printf(u8"无效套接字上的套接字操作。提供的套接字句柄无效。\n");
	default: printf(u8"未知错误 id = %d\n", id); break;
	};
}

/*
* 读取请求的一行
*/
int get_line(SOCKET sock, char* buff, int size) {
	if (buff == nullptr) {
		return 0;
	}

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
				// 预先读取，查看下一个字符是不是\n  但不会使套接字接收队列中的数据减少
				n = recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					n = recv(sock, &c, 1, 0); // 正常读取，下一个 \n
				}
				else {
					c = '\n';
				}
			}
			if (c != '\r' && c != '\n') {
				buff[i++] = c;
			}
			
		}
		else {
			getNetworkError();
			c = '\n';
		}
	}

	buff[i] = '\0'; // 结束符
	return i;
}

/*
* 从vector中读取一行
* @param body 请求体
* @param buff 存放读取到的内容
* @param index 开始读取的位置
* @return 最后读取到的位置
*/
int get_line(vector<char>& body, string& buff, int index) {
	size_t len = body.size(); // 数组长度
	char c = '\0'; // 保存读取到的一个字符
	int i = index; // 下标-  buff[i]

	while (i < len && c != '\n') {
		c = body[i++];
		if (c == '\r' && body[i] == '\n') {
			i++;
			break;
		}
		if (c != '\n' && c != '\r') {
			buff.push_back(c);
		}
	}

	return i;
}

/*
* 从vector<char>中读取一行，并保存在另外一个vector<char>中
* @param body 请求体
* @param buff 存放读取到的内容
* @param index 开始读取的位置
* @return 最后读取到的位置
*/
int get_PartBody_File(vector<char>& body, vector<char>& buff, int index) {
	size_t len = body.size(); // 数组长度
	char c = '\0'; // 保存读取到的一个字符
	int i = index; // 下标-  buff[i]

	while (i < len) {
		c = body[i++];
		if (c == '\r' && body[i] == '\n') {
			i++;
			break;
		}
		buff.push_back(c);
	}

	return i;
}

/**
*  解析multipart/form-data  part头
* @param body 请求体
* @param index 当前读取到的位置
* @param buff 读取每一行
* @param partHead 存放part头的map对象
* @return 读取到的位置
*/
int parsePartHead(vector<char>& body, int index, string& buff, unordered_map<string, string>& partHead) {
	int currentIndex = index;
	while (true)
	{
		// 读取Part头部
		buff.clear();
		currentIndex = get_line(body, buff, currentIndex);
		if (buff.size() == 0) break;
		/*
		* Content-Disposition: form-data; name="file"; filename="92cebf.jpg"
		*/
		// 这里读取的是文件类型
		char* outer_ptr = NULL;
		char* outer_ptr_query = NULL;

		char* queryStr = (char*)buff.c_str(); // 需要被分割的字符数组
		char* chunk = strtok_s(queryStr, ";", &outer_ptr);

		while (chunk)
		{
			string cun(chunk);
			char* tmp = NULL;
			string key;
			string value;

			if (cun.find("=") != -1) { // name="file"  以等号分割
				tmp = strtok_s(chunk, "=", &outer_ptr_query);
				key = tmp;
				delete_space(key);

				tmp = strtok_s(NULL, "=", &outer_ptr_query);
				value = tmp;
				delete_space(value); // 去除字符串两端空格
				delete_quotation(value); // 去除字符串两端引号
			}
			else { // Content-Disposition: form-data  以冒号分割
				tmp = strtok_s(chunk, ":", &outer_ptr_query);
				key = tmp;
				delete_space(key);// 去除字符串两端空格

				tmp = strtok_s(NULL, ":", &outer_ptr_query);
				value = tmp;
				delete_space(value);// 去除字符串两端空格
			}


			chunk = strtok_s(NULL, ";", &outer_ptr); // 以；分割

			partHead.emplace(key, value); // 保存part头
		}
	}

	return currentIndex;
}

/**
* 解析multipart/form-data  part 二进制文件内容
* @param body 数据源
* @param buff 存放数据的
* @param index 开始读取的位置
* @param startFlag boundary分割标志
* @param endFlag boundary分割标志
* @return 读取结束的位置
*/
int parsePartBody_File(vector<char>& body, vector<char>& buff, int index, string& startFlag, string& endFlag) {

	int currentIndex = index; // 记录当前读取到的位置
	auto first = body.begin() + currentIndex;
	auto end = body.end();
	
	vector<char> s(first, end);
	int flag = Algorithm::findSubstring(s, startFlag);

	while (currentIndex < (flag + index))
	{
		buff.push_back(body[currentIndex++]); // 保存文件
	}
	// 读取到最后位置会有\r\n这两个字符需要去除
	buff.pop_back(); // 去除\n
	buff.pop_back(); // 去除\r
	return currentIndex;
}

/**
* 读取请求体-文本
* @param sock 套接字
* @param read_count 读取多少个字节
* @param body 保存读取到的数据
* @return 一共读取了多少数据
*/
int get_body(SOCKET sock, int read_count,string& body) {
	char c = '\0'; // 保存读取到的一个字符
	int i = 0;
	while (i < read_count)
	{
		// 从缓冲区中读取一个字符
		int n = recv(sock, &c, 1, 0);
		// recv函数返回其实际copy的字节数，大于0表示读取到了字符
		if (n > 0) {
			body.push_back(c);
			i++;
		}
		else {
			getNetworkError();
		}
	}

	return i;
}

/**
* 读取请求体-二进制
* @param sock 套接字
* @param read_count 读取多少个字节
* @param body 保存读取到的数据
* @return 一共读取了多少数据
*/
int get_body(SOCKET sock, int read_count, vector<char>& body) {
	char c = '\0'; // 保存读取到的一个字符
	int i = 0; // 下标-  buff[i]

	while (i < read_count)
	{
		// 从缓冲区中读取一个字符
		int n = recv(sock, &c, 1, 0);
		// recv函数返回其实际copy的字节数，大于0表示读取到了字符
		if (n > 0) {
			body.push_back(c);
			i++;
		}
		else {
			getNetworkError();
			c = '\n';
		}
	}
	return i;
}

// 跳过空格
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

//去字符串两边的空格
void delete_space(string& s)
{
	if (s.empty())
	{
		return;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
}

// 去除string 两端引号
void delete_quotation(string& s) {
	if (s.empty())
	{
		return;
	}
	s.erase(0, s.find_first_not_of("\""));
	s.erase(s.find_last_not_of("\"") + 1);
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
	size_t len = strlen(path);
	size_t i = len;
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
		printf("MIME 获取失败: %s \n", e.what());
		value = "text/plain";
	}

	strcpy(type, value.c_str());

	//printf("文件类型是: %s \n", type);

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
	
	printf(u8"开始读取\n");
	while (true)
	{
		count = fread(buf, sizeof(char), sizeof(buf) - 1, fp);
		if (count == 0) {
			break;
		}
		buf[count] = '\0';

		fileStr.insert(fileStr.length(), buf, count);
	}
	printf(u8"一共读取到：%lld字节\n", fileStr.length());
	fclose(fp);
	
	//压缩：
	int nLencompressed = stringToGZIP(fileStr);

	const char* pCompressed = fileStr.c_str();

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

	printf(u8"一共写入: %lld \n" ,count);
	fclose(fp);
	delete[] pCompressed;
	return true;
}

// 字符串gzip压缩
int stringToGZIP(string& str)
{
	GZipAssistant* gzip = GetGZipAssistant();
	// 压缩前
	const char* pSrc = str.c_str();
	int nLenSrc = strlen(pSrc);
	cout << "压缩前: " << nLenSrc << endl;
	// 保存压缩后的数据
	int nLenCompress = nLenSrc * 2;
	char* pCompressed = new char[nLenCompress];
	memset(pCompressed, 0, nLenCompress);

	int nLencompressed = gzip->Compress(pSrc, nLenSrc, pCompressed, nLenCompress);
	cout << "压缩后: " << nLencompressed << endl;

	string gzipStr;
	for (int i = 0; i < nLencompressed; i++) {
		gzipStr.push_back(pCompressed[i]);
	}

	str.swap(gzipStr);

	gzipStr.clear();
	delete[] pCompressed;

	return nLencompressed;
}



// 页面未找到
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

// 打印map
void printMap(unordered_map<string, string>& map) {
	for (auto it = map.begin(); it != map.end(); it++) {
		cout << it->first << ":" << it->second << endl;
	}
}