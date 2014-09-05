#include "StdAfx.h"
#include "ProcessWatcher.h"
#include "Utility.h"
#include "DumpConfiguration.h"
#include "Security.h"

CProcessWatcher::CProcessWatcher():
m_hTd(NULL),
m_hStopEvent(CreateEventForExternalAccess(FALSE,FALSE,TEXT("EVENT_STOP_WATCH_PROCESSES"))),
m_hNewProcEvent(CreateEvent(NULL,FALSE,FALSE,NULL))
{
	InitializeCriticalSection(&this->m_lock);
}

CProcessWatcher& CProcessWatcher::GetWatcher()
{
	static CProcessWatcher watcher;
	return watcher;
}


CProcessWatcher::~CProcessWatcher()
{
	CloseHandle(m_hTd);
	CloseHandle(m_hStopEvent);
	CloseHandle(m_hNewProcEvent);
}


void CProcessWatcher::AddProcessHandle(HANDLE hProcess)
{
	{
		LockBlock lock(&m_lock);
		this->m_tmpList.push_back(hProcess);
	}

	::SetEvent(m_hNewProcEvent);
}

void CProcessWatcher::StartWatch()
{
	m_hTd = (HANDLE)_beginthreadex(NULL,0,CProcessWatcher::WorkFunc,this,0,NULL);
}

void CProcessWatcher::StopWatch()
{
	::SetEvent(m_hStopEvent);
	::WaitForSingleObject(m_hTd,INFINITE);
}

unsigned __stdcall CProcessWatcher::WorkFunc(void* pProcessWatcher)
{
	DebugLog(TEXT("Working thread started."));
	CProcessWatcher* pWatcher = reinterpret_cast<CProcessWatcher*>(pProcessWatcher);

	typedef std::map<HANDLE,std::tstring> ProcessHandleToImagePath;
	ProcessHandleToImagePath mapping;

	std::vector<HANDLE> vec;
	vec.push_back(pWatcher->m_hStopEvent);
	vec.push_back(pWatcher->m_hNewProcEvent);

	while(true)
	{
		{
			LockBlock lock(&pWatcher->m_lock);
			if(!pWatcher->m_tmpList.empty())
			{
				const size_t oldSize = vec.size();
				vec.resize(oldSize + pWatcher->m_tmpList.size());
				std::copy(pWatcher->m_tmpList.begin(), pWatcher->m_tmpList.end(), vec.begin() + oldSize);
				for(std::list<HANDLE>::iterator itor = pWatcher->m_tmpList.begin();
					itor != pWatcher->m_tmpList.end(); ++itor)
				{
					HANDLE hProcess = *itor;
					std::tstring imgPath = GetProcessImageFile(hProcess);
					mapping.insert(make_pair(hProcess,imgPath));
				}

				pWatcher->m_tmpList.clear();
			}
		}

		DebugLog1(TEXT("%d processes are under monitor"), vec.size() - 2);
		const DWORD dwWait = ::WaitForMultipleObjects(vec.size(), &vec[0], FALSE, INFINITE);
		switch(dwWait)
		{

		case WAIT_OBJECT_0:
			{
				DebugLog(TEXT("Notified to stop working."));
			}break;
		case WAIT_OBJECT_0 + 1:
			{
				DebugLog(TEXT("Notified to check new registered processes."));

			}break;
		case WAIT_FAILED:
			{
				const DWORD dwErr = GetLastError();
				DebugLog1(TEXT("Wait failed, error code = %d"), dwErr);
			}break;
		default:
			{
				const size_t index = dwWait - WAIT_OBJECT_0;
				HANDLE hProc = vec[index];
				DWORD dwExitCode = 0;
				::GetExitCodeProcess(hProc,&dwExitCode);
				DebugLog1(TEXT("Process exited with code = %d."),dwExitCode);
				if(dwExitCode != 0) 
				{
					// May be a crash happened, check the exit code 0 is not a accurate way,
					// need to improve in the future

					ProcessHandleToImagePath::const_iterator citor = mapping.find(hProc);
					const std::tstring imagePath = citor != mapping.end() ? citor->second : TEXT("");
					if(imagePath.empty())
					{
						DebugLog(TEXT("Failed to get process imange, will not check dump."));
					}
					else
					{
						std::tstring dir;
						std::tstring name;

						PROCESS_HANDLE hCurProc = ::GetCurrentProcess();
						SplitFullPath(::GetProcessImageFile(hCurProc),dir,name);
						if(!dir.empty())
						{
							const static TCHAR UPLOADER[] = TEXT("\\DumpUploader.exe");
							const std::tstring uploader = dir + UPLOADER;

							//It's better impersonate NETWORK SERVICE account here...
							PROCESS_INFORMATION piProcInfo = {0}; 
							STARTUPINFO siStartInfo = {0};   
							siStartInfo.cb = sizeof(STARTUPINFO);

							TCHAR tszCmdBuf[MAX_PATH * 2] = {0};
							_sntprintf_s(tszCmdBuf,_countof(tszCmdBuf),TEXT("%s /Collect %s"),uploader.c_str(), imagePath.c_str());

							DebugLog1(TEXT("Launching upload process ,command line: %s"),tszCmdBuf);
							if(!::CreateProcess(NULL, tszCmdBuf,NULL,NULL,FALSE,0,NULL,NULL,&siStartInfo,&piProcInfo))
							{
								DWORD err = ::GetLastError();
								DebugLog1(TEXT("Failed to launch upload process, error code = %d"),err);
							}

						}
					}
				}
				
				vec.erase(vec.begin() + index);
				mapping.erase(hProc);
				CloseHandle(hProc);
			}break;
		}

		if(dwWait == WAIT_OBJECT_0)
		{
			break;
		}
	}

	DebugLog(TEXT("Working thread exited."));
	return 0;
}
