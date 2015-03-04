// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// ------------------------------------- //

#include "stdafx.h"
#include <stdio.h>

#include "../dll/mydll.h"
#include "util.h"
#include "psapi.h"
#include "tlhelp32.h"
#include "ps.h"
#include "filter.h"

HINSTANCE g_hInstance =NULL;
CFilter g_Filter;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance =(HINSTANCE)hModule;

    return TRUE;
}

typedef BOOL (WINAPI *EnumProcessesFunc)(DWORD *, DWORD, DWORD *);

BOOL WINAPI myEnumProcesses(DWORD *lpidProcess,DWORD cb, DWORD *cbNeeded)
{
	WriteLog("EnumProcesses");

	return EnumProcesses(lpidProcess, cb, cbNeeded);
}

DWORD WINAPI myNtQuerySystemInformation(DWORD d1, PDWORD pd1, DWORD d2, PDWORD p1)
{
	//WriteLog("NtQuerySystemInformation, d1=%d,d2 =%d", d1, d2);
	HMODULE hMod =GetModuleHandle("ntdll.dll");

	NTQUERYSYSTEMINFORMATION NtQuerySystemInformation;
	NtQuerySystemInformation =(NTQUERYSYSTEMINFORMATION) GetProcAddress(hMod, "NtQuerySystemInformation");
	DWORD ret =NtQuerySystemInformation(d1, pd1, d2, p1);
	int err=GetLastError();

	if(d1 ==5)
	{
		char fname[128];
		PROCESS_INFO *p1, *p2, *p =(PROCESS_INFO *)pd1;
		
		p1 =p;
		p2 =NULL;
		while(((DWORD)p-(DWORD)pd1 <(DWORD)d2) && p1->NextEntryDelta !=0)
		{
			if(p->ProcessName.Buffer !=NULL)
			{
				int len =WideCharToMultiByte( CP_ACP, 0, p->ProcessName.Buffer, -1, fname, 128,NULL,NULL); 
				fname[len] =0;
				//WriteLog("NtQuery:fname=%s", fname);
				if(g_Filter.FilterName(fname))
				{
					if(p2) p2->NextEntryDelta +=p->NextEntryDelta;
					else
					{
						//if(p->NextEntryDelta)
						p1->NextEntryDelta += p->NextEntryDelta;
						p2 =p1;
					}
					//WriteLog("*********NtQuery:fname=%s", fname);
				}
				else p2 =NULL;
				//WriteLog("NtQuery:fname=%s", fname);
			}
			p1 =p;
			p =(PROCESS_INFO *)((char *)p+p->NextEntryDelta);
		}
		if(p2)
			p2->NextEntryDelta =0;
		//WriteLog("okkkkkkkkkkkkkkkkkkkkkkkkkkkkkk,p2=%x", p2);
	}
	SetLastError(err);

	return ret;
}

WNDENUMPROC g_old_enumproc =NULL;
typedef UINT (WINAPI *FuncGetWindowModuleFileNameA)(HWND hWnd, char *name, UINT);

BOOL CALLBACK myWndEnumProc(HWND hWnd, LPARAM lParam)
{
	char appname[50];
	
	appname[0] =0;
	
	GetFileNameFromhWnd(hWnd, appname);

	if(appname[0] ==0 || !g_Filter.FilterName(appname))
	{
		 if(g_old_enumproc) g_old_enumproc(hWnd, lParam);
	}

	return true;
}

BOOL WINAPI myEnumWindows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
	g_old_enumproc =lpEnumFunc;

	return EnumWindows(myWndEnumProc, lParam);
}

MYAPIINFO myapi_info[] =
{
	//{"PSAPI.DLL", "EnumProcesses(DWORD *,DWORD,DWORD *)", "myEnumProcesses"},
	{"ntdll.dll", "NtQuerySystemInformation", 4, "myNtQuerySystemInformation"},
	{"user32.dll", "EnumWindows", 2, "myEnumWindows"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
