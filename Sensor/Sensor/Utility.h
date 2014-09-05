#pragma once

const static size_t BUFFER_SIZE = 512;

#define MAKE_DEBUG_FORMAT(format) (TEXT("Sensor Log:") format)

#define DebugLog(format) \
{ \
	OutputDebugString(MAKE_DEBUG_FORMAT(format)); \
}



#define DebugLog1(format,v1) \
{ \
	TCHAR buf[BUFFER_SIZE] = {0}; \
	_sntprintf_s(buf, _countof(buf) - 1, MAKE_DEBUG_FORMAT(format),v1); \
	OutputDebugString(buf); \
}



#define DebugLog2(format,v1,v2) \
{ \
	TCHAR buf[BUFFER_SIZE] = {0}; \
	_sntprintf_s(buf, _countof(buf) - 1, MAKE_DEBUG_FORMAT(format),v1,v2); \
	OutputDebugString(buf); \
}

// Get full path of the specified process
inline
std::tstring GetProcessImageFile(HANDLE hProc)
{
	TCHAR szImagePath[MAX_PATH] = {0};
	DWORD dwSize = _countof(szImagePath) - 1;
	if(QueryFullProcessImageName(hProc,NULL,szImagePath,&dwSize))
	{
		return szImagePath;
	}
	else
	{
		DWORD err = GetLastError();
		DebugLog1(TEXT("Failed to get process image error code = %d"), err);
		return TEXT("");
	}
}

void SplitFullPath(const std::tstring& fullPath, std::tstring& dir, std::tstring& name);