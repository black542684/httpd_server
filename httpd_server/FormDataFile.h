#pragma once
#ifndef FORM_DATA_FILE
#define FORM_DATA_FILE
#include <string>
#include <vector>

using namespace std;
/*
* form-data 文件类型
*/
class FormDataFile
{
public:
	string contentDisposition;
	string contentType;
	string name; // 字段名
	string filename; // 上传的文件名称
	vector<char> value; // 文件
};

#endif // !FORM_DATA_FILE



