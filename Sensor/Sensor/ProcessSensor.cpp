// ProcessSensor.cpp : Implementation of CProcessSensor

#include "stdafx.h"
#include "ProcessSensor.h"
#include "Utility.h"
#include "ProcessWatcher.h"
#include "DumpConfiguration.h"
#include "Security.h"

// CProcessSensor

STDMETHODIMP CProcessSensor::RegisterProcess(LONG ProcessID)
{
	HANDLE hProcess = ::OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION ,FALSE,ProcessID);

	if(hProcess != NULL)
	{
		std::tstring imgPath = GetProcessImageFile(hProcess);
		if(!imgPath.empty())
		{
			std::tstring dir;
			std::tstring name;
			::SplitFullPath(imgPath, dir, name);

			if(!CDumpConfiguration::IsRegistered(name))
			{
				CDumpConfiguration dmpCfg(name);
				dmpCfg.Save();
			}
			DebugLog1(TEXT("Open process %s"),name.c_str());
			CProcessWatcher::GetWatcher().AddProcessHandle(hProcess);
		}
	}
	else
	{
		DWORD lastError = ::GetLastError();
		DebugLog2(TEXT("Open process %d failed, error code is %d"), ProcessID, lastError);
	}

	return S_OK;
}


STDMETHODIMP CProcessSensor::UnregisterProcess(LONG ProcessID)
{
	// TODO: Add your implementation code here

	DebugLog(TEXT("UnregisterProcess has not been implemented yet"));

	return S_OK;
}