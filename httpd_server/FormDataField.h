#pragma once
#ifndef FORM_DATA_FIELD
#define FORM_DATA_FIELD
#include <string>

using namespace std;

/*
* form-data �ֶ�����
*/
class FormDataField
{
public:
	string contentDisposition;
	string contentType;
	string name; // �ֶ���
	string value;// �ֶ�ֵ
};

#endif // !FORM_DATA_FIELD



