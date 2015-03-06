// HookDemo.cpp : Defines the entry point for the console application.
//
///////////////////////////////////////////////////////
// Andriy Oriekhov. 2006. Toleron Sofware.
// www.toleron.com
///////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include "monitoring.h"

int _tmain(int argc, _TCHAR* argv[])
{
    SC_HANDLE hServiceDB;
	SC_HANDLE schService;
	char szDriverFileName[MAX_PATH]={0};

    hServiceDB=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);

	GetModuleFileName( NULL,szDriverFileName,MAX_PATH);
	char * ReplaceFromPos = strrchr(szDriverFileName,'\\');
	szDriverFileName[ReplaceFromPos - szDriverFileName + 1] = 0;
	strcat(szDriverFileName,"NtProcDrv.sys");


	schService = ::CreateService(
		hServiceDB, 
		"NTProcDrv", 
		"Kernel Mode service",
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		szDriverFileName, 
		NULL, 
		NULL,
		NULL, 
		NULL, 
		NULL
		);

	if (schService == NULL)
	{
		printf("Cannot create service. ErrorCode = 0x%x\n",GetLastError());
		return 0;
	}


	if (!StartService(schService, 0, NULL))
	{
		printf("Cannot start service. ErrorCode = 0x%x\n",GetLastError());
		return 0;
	}

	CMonitoring Monitoring;

	printf("Press Enter to set hook on CreateProcess\n");
	int someint = getc(stdin);
    Monitoring.BeginMonitoring();

	printf("Press Enter to unhook\n");
	someint = getc(stdin);

	Monitoring.EndMonitoring();

	SERVICE_STATUS ServiceStatus;

	ControlService(schService,SERVICE_CONTROL_STOP,&ServiceStatus);
	DeleteService(schService);
	CloseServiceHandle(schService);


	return 0;
}

