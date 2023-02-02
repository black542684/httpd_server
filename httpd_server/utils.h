#pragma once
#ifndef UTILS
#define UTILS
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <stdexcept>

using namespace std;

/*
* ���ߺ���
*/

// ��ȡ���������Ϣ
void getNetworkError();

// ��ȡ�����һ��
int get_line(SOCKET sock, char* buff, int size);

// ��ȡ������
int get_body(SOCKET sock, int read_count,string& body);

// �����ո�
void trimStart(char* str, int size, int* index);

// ȥ��string ���˿ո�
void delete_space(std::string& s);

// �����������
void parseQuery(unordered_map<string, string> &query, char* queryStr);

// ��ȡMIME����
char* getContentType(const char* path);

// �ļ�ת��gzip
bool fileToGZIP(string filePath);

// ҳ��δ�ҵ�
void not_found(SOCKET client);

// ��ӡmap
void printMap(unordered_map<string, string> &map);

#endif // !UTILS
