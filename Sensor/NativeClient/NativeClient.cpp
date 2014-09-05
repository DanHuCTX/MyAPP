// NativeClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#import "..\Sensor\Debug\Sensor.tlb"

namespace
{
	void RegisterToSensor()
	{
		::CoInitialize(NULL);

		SensorLib::IProcessSensor* pSensor = NULL;
		HRESULT hr = ::CoCreateInstance(__uuidof(SensorLib::ProcessSensor),
			NULL,
			CLSCTX_LOCAL_SERVER,
			__uuidof(SensorLib::IProcessSensor),(void**)&pSensor);
		
		pSensor->RegisterProcess(::GetCurrentProcessId());
		pSensor->Release();

		::CoUninitialize();
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	RegisterToSensor();
	char* p = NULL;
	*p = 'b';

	return 0;
}

