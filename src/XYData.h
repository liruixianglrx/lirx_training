#ifndef _XYDATA_H_
#define _XYDATA_H_

#ifdef _WIN32
#ifdef XYDATA_EXPORTS
#define XYDATA_API extern "C" __declspec(dllexport)
#else
#define XYDATA_API extern "C" __declspec(dllimport)
#endif
#else
#define XYDATA_API extern "C"
#endif

#include <vector>
#include <string>

typedef void* XYHandle;

XYDATA_API const char* XYGetVersion();
XYDATA_API XYHandle XYNew(const char* path);
XYDATA_API int XYDelete(XYHandle handle);
XYDATA_API int XYRead(XYHandle handle, const char* type, const char* code, int date);
XYDATA_API const unsigned char* XYGetData(XYHandle handle);
XYDATA_API size_t XYGetDataSize(XYHandle handle);
XYDATA_API int XYQuery(XYHandle handle, const char* type, const char* code, int date);
XYDATA_API int XYConvertToInt64(XYHandle handle, int rowsize, const int* cols, int ncols, int64_t* output, int noutput);

// ����������ĳ���������͵�ȫ�������������Ƶ�һ���ַ������У���NULL������
XYDATA_API const char* const* XYGetErrorTypes(XYHandle handle, const char* datatype);

// ��ѯ�����Ƿ����ָ���Ĵ�������֮һ��errortypes�Ǵ����������Ƶ�һ���ַ������У���NULL������
XYDATA_API bool XYQueryDataErrors(XYHandle handle, const char* datatype, const char* code, int date, const char** errortypes);

enum XYErrorCode
{
	EC_OK,
	EC_INIT,
	EC_DATA,
	EC_FATAL
};

class XYData
{
public:
	static std::string GetVersion()
	{
		return XYGetVersion();
	}

	static XYData* New(const std::string& path)
	{
		return (XYData*)XYNew(path.c_str());
	}

	virtual void Release() = 0;
	virtual XYErrorCode Read(const std::string& type, const std::string& code, int date, std::vector<unsigned char>& output) = 0;
	virtual XYErrorCode Query(const std::string& type, const std::string& code, int date) = 0;

	// ����������ĳ���������͵�ȫ�������������ơ�
	virtual const std::vector<std::string> GetErrorTypes(const std::string& datatype) = 0;

	// ��ѯ�����Ƿ����ָ���Ĵ�������֮һ������true��ʾ���ڴ���
	virtual bool QueryDataErrors(const std::string& datatype, const std::string& code, int date, const std::vector<std::string>& errortypes) = 0;
};

#endif
