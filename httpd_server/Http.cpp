#include "Http.h"
#include "public_method.h"

Http::Http()
{

}

Http::~Http()
{
}

// 设置静态资源存放路径
void Http::setStatic(string path)
{
	this->staticPath = path;
}

string Http::getStatic()
{
	return this->staticPath;
}

void Http::all(string path, HandleFUNC func)
{
	allMethods.emplace(path, func);
}

HandleFUNC Http::all(string path)
{
	if (allMethods.count(path)) {
		return allMethods[path];
	}
	return nullptr;
}

void Http::get(string url, HandleFUNC func)
{
	getMethods.emplace(url, func);
}

HandleFUNC Http::get(string path)
{
	if (getMethods.count(path)) {
		return getMethods[path];
	}
	return nullptr;
}

void Http::post(string url, HandleFUNC func)
{
	postMethods.emplace(url, func);
}

HandleFUNC Http::post(string path)
{
	if (postMethods.count(path)) {
		return postMethods[path];
	}
	return nullptr;
}

void Http::listen(unsigned short port)
{
	listenSocket(port);
}

Http* getHttp() {
	return Http::getInstance();
}