// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// ------------------------------------- //

#include "stdafx.h"
#include <stdio.h>

#include "mydll.h"
#include "util.h"
#include "psapi.h"
#include "tlhelp32.h"
#include "ps.h"

HINSTANCE g_hInstance =NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance =(HINSTANCE)hModule;

    return TRUE;
}

// 截获执行notepad.exe文件的例子，在explorer中点击winnt\notepad.exe，然后
// 使用任务管理器查看进程，就发现有个notepad2.exe进程

DWORD WINAPI myCreateProcessW(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	char cmd[600];
	int len =WideCharToMultiByte( CP_ACP, 0, lpCommandLine, -1, cmd, sizeof(cmd),NULL,NULL); 
	cmd[len] =0;

	WriteLog("CreateProcessW :cmd=%s", cmd);
	strtok(cmd, " ");  //去掉空格
	strupr(cmd);
	if(cmd[0] =='\"')  // 去掉引号
	{
		memmove(cmd, &cmd[1], strlen(cmd)-1);
		cmd[strlen(cmd)-2] =0;
	}

	if(strstr(cmd, "NOTEPAD.EXE"))  // 复制文件并将命令定位到c:\notepad2.exe
	{
		WriteLog("found ok, cmd=%s", cmd);
		CopyFile(cmd, "c:\\notepad2.exe", 0);  //可以在此处解密notepad.exe为notepad2.exe
		strcpy(cmd, "c:\\notepad2.exe");
	}
	WCHAR wcmd[600];
	len =MultiByteToWideChar( CP_ACP, 0, cmd, strlen(cmd), wcmd, sizeof(wcmd)); 
	wcmd[len] =0;

	BOOL ifsuccess = CreateProcessW(NULL/*lpApplicationName*/,
		wcmd, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	DWORD err =GetLastError();
	SetLastError(err);
	return (DWORD)ifsuccess;
}

MYAPIINFO myapi_info[] =
{
	{"KERNEL32.DLL", "CreateProcessW(1,2,3,4,5,6,7,8,9,10)", "myCreateProcessW"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
