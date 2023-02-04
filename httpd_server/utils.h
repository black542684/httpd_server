#pragma once
#ifndef UTILS
#define UTILS
#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>

using namespace std;

/*
* ���ߺ���
*/

// ��ȡ���������Ϣ
void getNetworkError();

// ��ȡ�����һ��
int get_line(SOCKET sock, char* buff, int size);

/*
* ��vector<char>�ж�ȡһ��
* @param body ������
* @param buff ��Ŷ�ȡ��������
* @param index ��ʼ��ȡ��λ��
* @return ����ȡ����λ��
*/
int get_line(vector<char>& body, string& buff, int index);

/*
* ��vector<char>�ж�ȡһ�У�������������һ��vector<char>��
* @param body ������
* @param buff ��Ŷ�ȡ��������
* @param index ��ʼ��ȡ��λ��
* @return ����ȡ����λ��
*/
int get_PartBody_File(vector<char>& body, vector<char>& buff, int index);

/**
*  ����multipart/form-data  partͷ
* @param body ������
* @param currentIndex ��ǰ��ȡ����λ��
* @param buff ��ȡÿһ��
* @param partHead ���partͷ��map����
* @return ��ȡ����λ��
*/
int parsePartHead(vector<char>& body, int currentIndex, string& buff, unordered_map<string, string>& partHead);

/**
* ����multipart/form-data  part �������ļ�����
* @param body ����Դ
* @param buff ������ݵ�
* @param index ��ʼ��ȡ��λ��
* @param startFlag boundary�ָ��־
* @param endFlag boundary�ָ��־
* @return ��ȡ������λ��
*/
int parsePartBody_File(vector<char>& body, vector<char>& buff, int index, string& startFlag, string& endFlag);

// ��ȡ������-�ı�
int get_body(SOCKET sock, int read_count,string& body);

// ��ȡ������-������
int get_body(SOCKET sock, int read_count, vector<char>& body);

// �����ո�
void trimStart(char* str, int size, int* index);

// ȥ��string ���˿ո�
void delete_space(string& s);

// ȥ��string ��������
void delete_quotation(string& s);

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
