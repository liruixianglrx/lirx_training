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

// 返回适用于某个数据类型的全部错误类型名称的一个字符串序列，以NULL结束。
XYDATA_API const char* const* XYGetErrorTypes(XYHandle handle, const char* datatype);

// 查询数据是否包含指定的错误类型之一。errortypes是错误类型名称的一个字符串序列，以NULL结束。
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

	// 返回适用于某个数据类型的全部错误类型名称。
	virtual const std::vector<std::string> GetErrorTypes(const std::string& datatype) = 0;

	// 查询数据是否包含指定的错误类型之一。返回true表示存在错误。
	virtual bool QueryDataErrors(const std::string& datatype, const std::string& code, int date, const std::vector<std::string>& errortypes) = 0;
};

#endif
