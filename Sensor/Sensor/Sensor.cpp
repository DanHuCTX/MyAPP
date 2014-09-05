// Sensor.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "Sensor_i.h"
#include "ProcessWatcher.h"

#include <stdio.h>

class CSensorModule : public ATL::CAtlServiceModuleT< CSensorModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_SensorLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SENSOR, "{31D652E6-1DAA-4962-A843-99DA62D4A245}")
	HRESULT InitializeSecurity() throw()
	{
		// TODO : Call CoInitializeSecurity and provide the appropriate security settings for your service
		// Suggested - PKT Level Authentication, 
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY 
		// and an appropiate Non NULL Security Descriptor.
		return CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE,RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL );
	}

    HRESULT RegisterAppId(bool bService = false) throw ();
    HRESULT PreMessageLoop(int nShowCmd) throw();
    HRESULT PostMessageLoop() throw();
    void OnStop() throw();
    void OnPause() throw();
    void OnContinue() throw();
};

HRESULT CSensorModule::RegisterAppId(bool bService ) throw ()
{
    HRESULT hr = S_OK;
    BOOL res = __super::RegisterAppId(bService);
    if (bService)
    {
        if (IsInstalled())
        {
            SC_HANDLE hSCM = ::OpenSCManagerW(NULL, NULL, SERVICE_CHANGE_CONFIG);
            SC_HANDLE hService = NULL;
            if (hSCM == NULL)
            {
                hr = ATL::AtlHresultFromLastError();
            }
            else
            {
                hService = ::OpenService(hSCM, m_szServiceName, SERVICE_CHANGE_CONFIG);
                if (hService != NULL)
                {
                    ::ChangeServiceConfig(hService, SERVICE_NO_CHANGE,
                        SERVICE_DEMAND_START,
                        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                        m_szServiceName); // Loaded from IDS_SERVICENAME

                    SERVICE_DESCRIPTION Description;
                    TCHAR szDescription[1024];
                    ZeroMemory(szDescription, 1024);
                    ZeroMemory(&Description, sizeof (SERVICE_DESCRIPTION));
                    lstrcpy(szDescription, _T("Process lifecycle sensor" ));
                    Description.lpDescription = szDescription;
                    ::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &Description);
                    ::CloseServiceHandle(hService);
                }
                else
                {
                    hr = ATL::AtlHresultFromLastError();
                }
                ::CloseServiceHandle(hSCM);
            }
        }
    }
    return hr;
}

HRESULT CSensorModule::PreMessageLoop(int nShowCmd) throw()
{
    // Enable the pause and continue button
    m_status.dwControlsAccepted = m_status.dwControlsAccepted | SERVICE_ACCEPT_PAUSE_CONTINUE;

    HRESULT hr = __super::PreMessageLoop(nShowCmd);
    // Microsoft bugs
    if (hr == S_FALSE)
        hr = S_OK;

    // Add our initialization codes here

    if (SUCCEEDED(hr))
    {
        // Must set running state or we got 1053 error
        SetServiceStatus(SERVICE_RUNNING);
    }

	CProcessWatcher::GetWatcher().StartWatch();
    return hr;
}

HRESULT CSensorModule::PostMessageLoop() throw()
{
    HRESULT hr = __super ::PostMessageLoop();

    if (FAILED(hr))
        return hr;

    // Add our clean codes here

    return hr;
}

void CSensorModule::OnStop() throw()
{
	CProcessWatcher::GetWatcher().StopWatch();
    __super::OnStop();
    SetServiceStatus(SERVICE_STOPPED);
}

void CSensorModule::OnPause() throw()
{
    __super::OnPause();
    SetServiceStatus(SERVICE_PAUSED);
}

void CSensorModule::OnContinue() throw()
{
    __super::OnContinue();
    SetServiceStatus(SERVICE_RUNNING);
}


CSensorModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

