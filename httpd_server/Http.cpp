#include "Http.h"
#include "public_method.h"

Http::Http()
{

}

Http::~Http()
{
}

void Http::get(string url, HandleFUNC func)
{
	getMethods.emplace(url, func);
}

void Http::post(string url, HandleFUNC func)
{
	postMethods.emplace(url, func);
}

void Http::listen(unsigned short port)
{
	listenSocket(port);
}

Http* getHttp() {
	return Http::getInstance();
}