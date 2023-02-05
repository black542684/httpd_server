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
	string staticPath = "./"; // ��̬��Դ���·��
	unordered_map<string, HandleFUNC> allMethods; // ����all������
	unordered_map<string, HandleFUNC> getMethods; // ����GET������
	unordered_map<string, HandleFUNC> postMethods; // ����GET������
public:
	Request req; // �������
	Response res; // ��Ӧ����

public:
	// ���þ�̬��Դ���·��
	void setStatic(string path);
	string getStatic();

	// ��Ŵ�����������ĺ���ָ��
	void all(string, HandleFUNC);
	HandleFUNC all(string); // ��ȡ������������ĺ���ָ��

 	// ��Ŵ���GET����ĺ���ָ��
	void get(string, HandleFUNC); 
	HandleFUNC get(string);

	// ��Ŵ���POST����ĺ���ָ��
	void post(string, HandleFUNC); 
	HandleFUNC post(string);


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



