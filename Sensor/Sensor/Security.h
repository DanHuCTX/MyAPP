#pragma once

typedef BOOL (WINAPI *RELEASE)(UINT_PTR);
template <typename T, RELEASE ReleaseHandle = (RELEASE)&::CloseHandle, UINT_PTR InvalidHandle = NULL>
class auto_handle
{
public:
     auto_handle(T handle) : m_handle(handle) {}
     ~auto_handle() { if(!Invalid()) (*ReleaseHandle)((UINT_PTR)m_handle); }
     operator T()const { return m_handle; }
     bool Invalid() const { return m_handle == (T)InvalidHandle; }
	 T* operator &() {return &m_handle;}
private:
     T m_handle;
};

typedef auto_handle<HANDLE,(RELEASE)&::CloseHandle,NULL> PROCESS_HANDLE, PROCESS_TOKEN_HANDLE;

inline
bool EnableDebugPriviledge(bool enable)
{
	PROCESS_TOKEN_HANDLE hToken = NULL;
	OpenProcessToken(::GetCurrentProcess(),TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,&hToken);
	if(!hToken.Invalid())
	{
		TOKEN_PRIVILEGES tokenPrivileges;
		tokenPrivileges.PrivilegeCount = enable ? 1 : 0;
		tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if(LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tokenPrivileges.Privileges[0].Luid))
		{
			AdjustTokenPrivileges(hToken,FALSE,&tokenPrivileges,sizeof(TOKEN_PRIVILEGES),NULL,NULL);

			return ::GetLastError() == ERROR_SUCCESS;
		}
	}

	return false;
}

inline 
HANDLE CreateEventForExternalAccess(BOOL bManualReset, BOOL bInitialState,const TCHAR* ctszEventName)
{
	SECURITY_DESCRIPTOR SecurityDescriptor = { 0 };
	::InitializeSecurityDescriptor(&SecurityDescriptor, 1);
	::SetSecurityDescriptorDacl(&SecurityDescriptor, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES SecurityAttribute = { 0 };
	SecurityAttribute.nLength               = sizeof(SecurityAttribute);
	SecurityAttribute.lpSecurityDescriptor = &SecurityDescriptor;
	SecurityAttribute.bInheritHandle        = TRUE;
	return ::CreateEventW(&SecurityAttribute, bManualReset, bInitialState,ctszEventName);
}
