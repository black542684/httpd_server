#pragma once
#ifndef CLASS_GZipAssistant
#define CLASS_GZipAssistant
#pragma comment(lib, "../zlib/debug/zlibwapi.lib")

// gzipѹ��
class GZipAssistant
{
public:
	GZipAssistant();
	~GZipAssistant();
public:
	// ѹ��
	int Compress(const char* src, int srcLen, char* dest, int destLen);
	// ��ѹ��
	int Decompress(const char* src, int srcLen, const char* dst, int dstLen);

	static GZipAssistant* GetInstance()
	{
		static GZipAssistant ins;
		return &ins;
	}
};

GZipAssistant* GetGZipAssistant();
#endif // !CLASS_GZipAssistant



