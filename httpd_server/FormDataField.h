#pragma once
#ifndef FORM_DATA_FIELD
#define FORM_DATA_FIELD
#include <string>

using namespace std;

/*
* form-data 字段类型
*/
class FormDataField
{
public:
	string contentDisposition;
	string contentType;
	string name; // 字段名
	string value;// 字段值
};

#endif // !FORM_DATA_FIELD



