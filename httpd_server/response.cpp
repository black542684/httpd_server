#include "response.h"

/**
* request: 请求
* folder: 默认文件夹
*/
Response::Response(Request& request, string folder)
{
	this->client = request.getSocket();
	this->folder = folder; // 设置文件夹基本路径
	this->gzip = request.isgzip(); // 是否开启gzip

	
	generatePath(request.getUrl());
	generateMIME();
	
	//if (mime == "text/html") { // HTML文件不用gzip
	//	this->gzip = false;
	//}
	//else {
	//	this->gzip = request.isgzip(); // 是否开启gzip
	//}

	generateHead();
}

// 发送响应头
void Response::sendHeaders()
{
	// 向指定套接字，发送一个响应头
	string responseLine = "HTTP/1.1 200 OK\r\n";
	::send(this->client, responseLine.c_str(), responseLine.length(), 0);

	for (auto it = head.begin(); it != head.end(); it++) {
		string line(it->first + ": " + it->second + "\r\n");
		::send(this->client, line.c_str(), line.length(), 0);
	}
	string over = "\r\n";
	::send(this->client, over.c_str(), over.length(), 0);
}

// 发送资源文件
void Response::snedFile(FILE* resource)
{
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
		// 向套接字发送数据
		::send(this->client, buff, ret, 0);
		count += ret;
	}

	printf("一共发送%lld个字节\n\n\n", count);
}

// 给客户端返回资源
void Response::send()
{
	// 向客户端发送文件
	int numchars = 1;
	char buff[1024] = { 0 };

	// 文件不存在
	if (this->res_is_empty == -1) {
		not_found(this->client);
	}
	else {
		FILE* resource = fopen(this->path.c_str(), "rb");
		this->sendHeaders();
		this->snedFile(resource);
	}
}

// 生成默认响应头
void Response::generateHead()
{
	this->head["Server"] = "z-httpd/1.1";

	if (this->mime == "text/html") {
		this->head["Content-Type"] = "text/html; charset=utf-8";
	}
	else {
		this->head["Content-Type"] = this->mime;
	}

	if (this->gzip) {
		this->head["Content-Encodin"] = "gzip";
	}

}

// 生成请求文件路径
void Response::generatePath(string url)
{
	if (url == "/") {
		this->path = this->folder + "/index.html";
	}
	else {
		this->path = this->folder + url;
		
	}
	// 判断path 路径的属性-文件or文件夹
	struct stat status;
	this->res_is_empty = stat(this->path.c_str(), &status);

	// 判断文件类型
	if ((status.st_mode & S_IFMT) == S_IFDIR) {
		
		// 如果是文件夹-则拼接  /docs/image index.html
		if (this->path[this->path.length() - 1] == '/') {
			this->path += "index.html";
		}
		else {
			this->path += "/index.html";
		}

	}


}

// 生成数据类型
void Response::generateMIME()
{
	string mime(getContentType(this->path.c_str()));
	this->mime = mime;
}
