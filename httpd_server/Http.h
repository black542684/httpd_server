#pragma once
#ifndef HTTP_CLASS
#define HTTP_CLASS
#include <unordered_map>
#include "request.h"
#include "response.h"

using namespace std;
typedef void (*HandleFUNC)(Request&, Response&);

// ��ʼ��HTTP������
class Http
{
public:
	~Http();
private:
	Http();
private:
	unordered_map<string, HandleFUNC> getMethods; // ����GET������
	unordered_map<string, HandleFUNC> postMethods; // ����GET������
public:
	// ��Ŵ���GET����ĺ���ָ��
	void get(string, HandleFUNC); 
	// ��Ŵ���POST����ĺ���ָ��
	void post(string, HandleFUNC); 
	// �����˿�
	void listen(unsigned short port);
private:

public:
	static Http* getInstance() { // ��ȡʵ��
		static Http ins;
		return &ins;
	}

};

Http* getHttp();

#endif // !HTTP_CLASS



