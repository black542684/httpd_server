#pragma once
#ifndef FORM_DATA_FILE
#define FORM_DATA_FILE
#include <string>
#include <vector>

using namespace std;
/*
* form-data �ļ�����
*/
class FormDataFile
{
public:
	string contentDisposition;
	string contentType;
	string name; // �ֶ���
	string filename; // �ϴ����ļ�����
	vector<char> value; // �ļ�
};

#endif // !FORM_DATA_FILE



