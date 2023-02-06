#pragma once
#ifndef CLASS_GZipAssistant
#define CLASS_GZipAssistant
#pragma comment(lib, "../zlib/lib/zlibwapi.lib")

// gzip压缩
class GZipAssistant
{
public:
	GZipAssistant();
	~GZipAssistant();
public:
	// 压缩
	int Compress(const char* src, int srcLen, char* dest, int destLen);
	// 解压缩
	int Decompress(const char* src, int srcLen, const char* dst, int dstLen);

	static GZipAssistant* GetInstance()
	{
		static GZipAssistant ins;
		return &ins;
	}
};

GZipAssistant* GetGZipAssistant();
#endif // !CLASS_GZipAssistant



