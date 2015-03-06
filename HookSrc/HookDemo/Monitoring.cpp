#include "stdafx.h"
#include <windows.h>
#include "Monitoring.h"
#include <winioctl.h>


#define FILE_DEVICE_UNKNOWN             0x00000022
#define IOCTL_UNKNOWN_BASE              FILE_DEVICE_UNKNOWN
#define IOCTL_NTPROCDRV_SET_ADDRNFO		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

CMonitoring::CMonitoring()
{
	m_hDriver = 0;
}

CMonitoring::~CMonitoring()
{
    EndMonitoring();
}


BOOL CMonitoring::BeginMonitoring(void)
{
    BOOL bResult=FALSE;

    char szDriverName[MAX_PATH];
	szDriverName[0] = 0;

	strcpy(szDriverName, OTHER_DRIVERNAME);				

    // Try opening the device driver
	m_hDriver = CreateFile(
		szDriverName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,                     // Default security
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,  // Perform asynchronous I/O
		0);                    // No template


	if(m_hDriver == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (!SetWindowsInfoToDriver(TRUE))
	{
		CloseHandle(m_hDriver);
		m_hDriver = NULL;
		return FALSE;
	}
	bResult = TRUE;
        				
    return bResult;
}

BOOL CMonitoring::EndMonitoring(void)
{
    BOOL bResult=FALSE;
    
	SetWindowsInfoToDriver(FALSE);

    if (m_hDriver != NULL)
	{
        ::CloseHandle(m_hDriver);
		m_hDriver = 0;
		bResult = TRUE;
	}
    return bResult;
}

unsigned CMonitoring::GetNtSSTIndex(LPCSTR szFuncName)
{
	unsigned nIndex = 0;
	BYTE * pFuncBody = (BYTE *)GetProcAddress(GetModuleHandle("NtDll.dll"),szFuncName);
	if (pFuncBody)
		nIndex = *(DWORD *)(pFuncBody+1);

	return nIndex;
}

BOOL CMonitoring::SetWindowsInfoToDriver(BOOL isStart)
{
	OVERLAPPED ov          = { 0 };
	BOOL       bReturnCode = FALSE;
	DWORD      dwBytesReturned;

    // Create an event handle for async notification from the driver
	ov.hEvent = ::CreateEvent(
		NULL,  // Default security
		TRUE,  // Manual reset
		FALSE, // non-signaled state
		NULL
		); 

	if (ov.hEvent == NULL)
	{
		return FALSE;
	}

	// Get the process info
	CAddrInfo AddrInfo = {0};
	AddrInfo.isInitialized = isStart;
	if (isStart)
	{
		AddrInfo.pfnLoadLibrary = GetProcAddress(GetModuleHandle("Kernel32.dll"),"LoadLibraryW");
		AddrInfo.SSTIndexNtCreateProcess = GetNtSSTIndex("NtCreateProcess");
		AddrInfo.SSTIndexNtCreateThread = GetNtSSTIndex("NtCreateThread");
		AddrInfo.SSTIndexNtTerminateProcess = GetNtSSTIndex("NtTerminateProcess");
		AddrInfo.SSTIndexNtCreateProcessEx = GetNtSSTIndex("NtCreateProcessEx");
	}
		

	bReturnCode = ::DeviceIoControl(
		m_hDriver,
		IOCTL_NTPROCDRV_SET_ADDRNFO,
		&AddrInfo, 
		sizeof(CAddrInfo),
		0,
		0,
		&dwBytesReturned,
		&ov);

	if (!bReturnCode)
	{
		return FALSE;
	}

	// Wait here for the event handle to be set, indicating
	// that the IOCTL processing is completed.
	bReturnCode = ::GetOverlappedResult(
		m_hDriver, 
		&ov,
		&dwBytesReturned, 
		TRUE
		);

	if (!bReturnCode)
	{
		return FALSE;
	}

	::CloseHandle(ov.hEvent);
	// Pevent getting duplicated events

	return TRUE;

}