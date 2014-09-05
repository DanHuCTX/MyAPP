#pragma once
#include "ConfigDefineMacros.h"

class CDumpConfiguration
{
public:
	CDumpConfiguration(const std::tstring& imgName, const TCHAR* dumpPath = NULL, unsigned long dumpType = 2):
	  m_strImageName(imgName),
		  m_strDumpPath(dumpPath ? dumpPath : TEXT("")),
		  m_uDumpType(dumpType)
	  {
	  }

	void Save();
	void Load();
	static bool IsRegistered(const std::tstring& imgName);

	DEFINE_PROPERTY_TSTRING(ImageName);
	DEFINE_PROPERTY_TSTRING(DumpPath);
	DEFINE_PROPERTY_ULONG(DumpType);
};