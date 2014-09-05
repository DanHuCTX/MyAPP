#include "stdafx.h"
#include "Utility.h"

void SplitFullPath(const std::tstring& fullPath, std::tstring& dir, std::tstring& name)
{
	std::tstring::size_type pos = fullPath.find_last_of((TCHAR)'\\');
	if(pos == std::tstring::npos || pos == fullPath.length() - 1)
	{
		dir = fullPath;
		name = TEXT("");
	}
	dir = fullPath.substr(0,pos);
	name = (pos != std::tstring::npos ? fullPath.substr(pos + 1) : TEXT(""));
}
