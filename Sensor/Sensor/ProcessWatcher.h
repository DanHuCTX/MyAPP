#pragma once
#include <list>

class LockBlock
{
public:
	LockBlock(LPCRITICAL_SECTION pLock):m_pLock(pLock)
	{
		EnterCriticalSection(m_pLock);
	}

	~LockBlock()
	{
		LeaveCriticalSection(m_pLock);
	}

private:
	LPCRITICAL_SECTION m_pLock;
};

class CProcessWatcher
{
public:
	static CProcessWatcher& GetWatcher();
	~CProcessWatcher();
	void AddProcessHandle(HANDLE hProcess);
	void StartWatch();
	void StopWatch();
private:
	CProcessWatcher();

private:
	mutable CRITICAL_SECTION m_lock;
	std::list<HANDLE> m_tmpList; //Temp proc list to be monitored
	HANDLE m_hTd;
	HANDLE m_hStopEvent;
	HANDLE m_hNewProcEvent;

private:
	static  unsigned __stdcall WorkFunc(void* pProcessWatcher);
};

