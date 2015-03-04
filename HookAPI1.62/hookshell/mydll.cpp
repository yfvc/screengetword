// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// ------------------------------------- //

#include "stdafx.h"
#include <stdio.h>
#include <shellapi.h>
#include "mydll.h"

#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

void WriteLog(char *fmt, ...)
{
	va_list args;
	char modname[200];

	char temp[5000];
	HANDLE hFile;

	GetModuleFileName(NULL, modname, sizeof(modname));

	if((hFile = CreateFile("c:\\hookapi.log", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0)
	{
		return;
	}
	
	_llseek((HFILE)hFile, 0, SEEK_END);

	wsprintf(temp, "mydll.dll:%s:", modname);
	DWORD dw;
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	
	va_start(args,fmt);
	vsprintf(temp, fmt, args);
	va_end(args);

	WriteFile(hFile, temp, strlen(temp), &dw, NULL);

	wsprintf(temp, "\r\n");
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);

	_lclose((HFILE)hFile);
}

BOOL GetCurProcessBaseName(char *pBaseName, int BufSize)
{
	BOOL bRet = FALSE;
	char szCurProcessPathName[MAX_PATH];
	memset(szCurProcessPathName, 0, MAX_PATH);
	memset(pBaseName, 0, BufSize);

	::GetModuleFileName(NULL, szCurProcessPathName, MAX_PATH);
	char *p = strrchr(szCurProcessPathName, '\\');
	if(p)
	{
		strcpy(pBaseName, p + 1);
		bRet = TRUE;
	}

	return bRet;
}

FARPROC WINAPI MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
/*
	char szCurProcessBaseName[MAX_PATH];
	
	if(GetCurProcessBaseName(szCurProcessBaseName, MAX_PATH))
	{
		if(stricmp(szCurProcessBaseName, "explorer.exe") == 0)
		{
			WriteLog("You Use GetProcAddress, Func Name: %s", lpProcName);
		}
	}
*/	
	return GetProcAddress(hModule, lpProcName);
}

HMODULE WINAPI MyLoadLibraryA(LPCSTR lpLibFileName)
{
/*
	char szCurProcessBaseName[MAX_PATH];
	
	if(GetCurProcessBaseName(szCurProcessBaseName, MAX_PATH))
	{
		if(stricmp(szCurProcessBaseName, "explorer.exe") == 0)
		{
			WriteLog("You Use LoadLibraryA, Dll Name: %s", lpLibFileName);
		}
	}
*/
	return LoadLibraryA(lpLibFileName);
}

HMODULE WINAPI MyLoadLibraryW(LPCWSTR lpLibFileName)
{
/*
	char fname[MAX_PATH];
	memset(fname, 0, MAX_PATH);
	::WideCharToMultiByte( CP_ACP, 0, lpLibFileName, -1, fname, MAX_PATH, NULL, NULL);
	
	char szCurProcessBaseName[MAX_PATH];
	if(GetCurProcessBaseName(szCurProcessBaseName, MAX_PATH))
	{
		if(stricmp(szCurProcessBaseName, "explorer.exe") == 0)
		{
//			WriteLog("You Use LoadLibraryW, Dll Name: %s", fname);
		}
	}
*/	
	return LoadLibraryW(lpLibFileName);
}

int WINAPI MySHFileOperation(LPSHFILEOPSTRUCTA lpFileOp)
{
	//WriteLog("You Use SHFileOperation");
	
//	if(strstr(lpFileOp->pFrom, "Test.txt") != NULL)
//		return 0;
//	else
		return SHFileOperation(lpFileOp);
}

int WINAPI MySHFileOperationA(LPSHFILEOPSTRUCTA lpFileOp)
{
	WriteLog("You Use SHFileOperationA");
	if(lpFileOp ==NULL)
		return 0;
	
	if(strstr(lpFileOp->pFrom, "Test.txt") != NULL)
		return 0;
	else
		return SHFileOperationA(lpFileOp);
}

int WINAPI MySHFileOperationW(LPSHFILEOPSTRUCTW lpFileOp)
{
	WriteLog("You Use SHFileOperationW");
	if(lpFileOp ==NULL)
		return 0;
	
	char fname[MAX_PATH];
	memset(fname, 0, MAX_PATH);
	::WideCharToMultiByte( CP_ACP, 0, lpFileOp->pFrom, -1, fname, MAX_PATH, NULL, NULL);
	if(strstr(fname, "Test.txt") != NULL)
		return 0;
	else
		return SHFileOperationW(lpFileOp);
	
	return 0;
}

MYAPIINFO myapi_info[] =
{
//	{"KERNEL32.DLL", "GetProcAddress", 2, "MyGetProcAddress"},
//	{"KERNEL32.DLL", "LoadLibraryA", 1, "MyLoadLibraryA"},LoadLibraryA
//	{"KERNEL32.DLL", "LoadLibraryW", 1, "MyLoadLibraryW"},
	{"Shell32.DLL", "SHFileOperation", 1, "MySHFileOperation"},
	{"Shell32.DLL", "SHFileOperationA", 1, "MySHFileOperationA"},
	{"Shell32.DLL", "SHFileOperationW", 1, "MySHFileOperationW"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
