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

void WriteLog(char *fmt,...)
{
	va_list args;
	char modname[200];

	char temp[5000];
	HANDLE hFile;

	GetModuleFileName(NULL, modname, sizeof(modname));

	if((hFile =CreateFile("c:\\hookapi.log", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0)
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

HANDLE WINAPI myCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{          
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	WriteLog("%s, myCreateFileA:filename=%s", temp, lpFileName);
	//MessageBox(NULL, temp, "mydll", MB_OK);
	if(strstr(lpFileName, "aaa") !=NULL) return NULL;
	return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI myCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	//MessageBox(NULL, "myCreateFileW", "ok", MB_OK);
                              
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	char fname[128];
	WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, fname, 128,NULL,NULL); 

	WriteLog("%s, myCreateFileW:filename=%s", temp, fname);
	//MessageBox(NULL, temp, "mydll", MB_OK);
	
	if(strstr(fname, "aaa.txt") !=NULL)
	{
		WriteLog("CreateFileW aaa found!");
		return CreateFileA("c:\\temp\\bbb.txt", dwDesiredAccess, dwShareMode, lpSecurityAttributes,
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		//SetLastError(ERROR_FILE_NOT_FOUND);
		//return NULL;
	}

	return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL WINAPI myDeleteFileA(LPCSTR lpFileName)
{
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	WriteLog("%s,\n myDeleteFileA:filename=%s", temp, lpFileName);
	if(strstr(temp, "aaa") !=NULL)
	{
		WriteLog("DeleteFileA aaa found!");
		SetLastError(ERROR_FILE_NOT_FOUND);
		return NULL;
	}

	return DeleteFileA(lpFileName);
}

BOOL WINAPI myDeleteFileW(LPCWSTR lpFileName)
{
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	char fname[128];
	WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, fname, 128,NULL,NULL); 
	WriteLog("%s,\n myDeleteFileW:filename=%s", temp, fname);
	if(strstr(fname, "aaa") !=NULL)
	{
		WriteLog("DeleteFileW aaa found!");
		SetLastError(ERROR_FILE_NOT_FOUND);
		return NULL;
	}

	return DeleteFileW(lpFileName);
}

BOOL WINAPI myReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	WriteLog("ReadFile:handle=%x", hFile);

	return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

BOOL WINAPI myReadFileEx(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
		LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	WriteLog("ReadFileEx");

	return ReadFileEx(hFile, lpBuffer, nNumberOfBytesToRead,
		lpOverlapped, lpCompletionRoutine);
}

BOOL WINAPI myWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
		LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	WriteLog("WriteFile");

	return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten,
		lpOverlapped);
}

BOOL WINAPI myWriteFileEx(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
		LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	WriteLog("WriteFileEx");

	return WriteFileEx(hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);
}

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

	BOOL ifsuccess = CreateProcessW(lpApplicationName,
		lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	DWORD err =GetLastError();
	SetLastError(err);
	
	return (DWORD)ifsuccess;
}

DWORD WINAPI myCreateProcessA(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFO lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{

	BOOL ifsuccess = CreateProcessA(lpApplicationName,
		lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	DWORD err =GetLastError();
	WriteLog("CreateProcessA %s", lpCommandLine);
	SetLastError(err);

	return (DWORD)ifsuccess;
}

MYAPIINFO myapi_info[] =
{
	{"KERNEL32.DLL", "CreateFileA", 7, "myCreateFileA"},
	{"KERNEL32.DLL", "CreateFileW", 7, "myCreateFileW"},
	{"KERNEL32.DLL", "DeleteFileA", 1, "myDeleteFileA"},
	{"KERNEL32.DLL", "DeleteFileW", 1, "myDeleteFileW"},
	{"KERNEL32.DLL", "ReadFile", 5, "myReadFile"},
	{"KERNEL32.DLL", "ReadFileEx", 5, "myReadFileEx"},
	{"KERNEL32.DLL", "WriteFile", 5, "myWriteFile"},
	{"KERNEL32.DLL", "WriteFileEx", 5, "myWriteFileEx"},
	{"KERNEL32.DLL", "CreateProcessW", 10, "myCreateProcessW"},
	{"KERNEL32.DLL", "CreateProcessA", 10, "myCreateProcessA"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
