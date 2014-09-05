#include "stdafx.h"
#include "DumpConfiguration.h"

namespace
{
	const static TCHAR* REG_WER_LOCAL_DUMP_PATH = TEXT("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps");

	inline bool RegkeyExisted(HKEY rootKey, const std::tstring& subKey)
	{
		ATL::CRegKey key;
		long result = key.Open(rootKey,subKey.c_str(),KEY_READ | KEY_WOW64_64KEY);
		return result == ERROR_SUCCESS;
	}
}

void CDumpConfiguration::Save()
{
	TCHAR buf[MAX_PATH] = {0};
	_sntprintf_s(buf, _countof(buf) - 1, TEXT("%s\\%s"), REG_WER_LOCAL_DUMP_PATH, m_strImageName.c_str());

	ATL::CRegKey key;
	key.Create(HKEY_LOCAL_MACHINE, buf, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE| KEY_WOW64_64KEY);

	if(m_strDumpPath.empty())
	{
		GetTempPath(_countof(buf) - 1, buf);
		_sntprintf_s(buf, _countof(buf) - 1, TEXT("%s%s"),buf, m_strImageName.c_str());
		m_strDumpPath = buf;
	}

	key.SetStringValue(TEXT("DumpFolder"), m_strDumpPath.c_str(),REG_EXPAND_SZ);
	key.SetDWORDValue(TEXT("DumpType"), m_uDumpType);

}

void CDumpConfiguration::Load()
{
	TCHAR buf[MAX_PATH] = {0};
	_sntprintf_s(buf, _countof(buf) - 1, TEXT("%s\\%s"), REG_WER_LOCAL_DUMP_PATH, m_strImageName.c_str());

	ATL::CRegKey key;
	key.Open(HKEY_LOCAL_MACHINE,buf,KEY_READ | KEY_WOW64_64KEY);

	unsigned long sBuf = _countof(buf) - 1;
	key.QueryStringValue(TEXT("DumpFolder"),buf,&sBuf);
	m_strDumpPath = buf;

	key.QueryDWORDValue(TEXT("DumpType"),m_uDumpType);
}

bool CDumpConfiguration::IsRegistered(const std::tstring& imgName)
{
	TCHAR buf[MAX_PATH] = {0};
	_sntprintf_s(buf, _countof(buf) - 1, TEXT("%s\\%s"), REG_WER_LOCAL_DUMP_PATH, imgName);
	return RegkeyExisted(HKEY_LOCAL_MACHINE, buf);
}