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
#include "filter.h"

HINSTANCE g_hInstance =NULL;
CFileFilter g_Filter;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance =(HINSTANCE)hModule;
    return TRUE;
}

HANDLE WINAPI myCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{          
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	//WriteLog("myCreateFileA:filename=%s", lpFileName);

	if(g_Filter.FilterCreateFile((char *)lpFileName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}
	return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI myCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	char fname[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, fname, 128,NULL,NULL); 
	fname[len] =0;

	WriteLog("myCreateFileW:filename=%s", fname);

	if(strstr(fname, "NOTEPAD") || strstr(fname, "notepad") || strstr(fname, "Notepad") || strstr(fname, "NotePad"))
	{
		WriteLog("found notepad");
		return NULL;
	}
	if(g_Filter.FilterCreateFile(fname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	HANDLE hFile =CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	int err =GetLastError();
	//WriteLog("CreateFileW:handle=%x", hFile);
	SetLastError(err);

	return hFile;
}

BOOL WINAPI myDeleteFileA(LPCSTR lpFileName)
{
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	WriteLog("myDeleteFileA:filename=%s", lpFileName);

	if(g_Filter.FilterDeleteFile((char *)lpFileName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return DeleteFileA(lpFileName);
}

BOOL WINAPI myDeleteFileW(LPCWSTR lpFileName)
{
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	char fname[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, fname, 128,NULL,NULL); 
	fname[len] =0;
	WriteLog("myDeleteFileW:filename=%s", fname);

	if(g_Filter.FilterDeleteFile(fname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return DeleteFileW(lpFileName);
}

BOOL WINAPI myCopyFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists)
{
	char fname1[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpExistingFileName, -1, fname1, 128,NULL,NULL); 
	fname1[len] =0;
	char fname2[128];
	len =WideCharToMultiByte( CP_ACP, 0, lpNewFileName, -1, fname2, 128,NULL,NULL); 
	fname2[len] =0;

	WriteLog("CopyfileW:%s to %s", fname1, fname2);

	if(g_Filter.FilterCopyFile(fname1, fname2))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}
	
	return CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL WINAPI myCopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists)
{
	WriteLog("CopyfileA:%s to %s", lpExistingFileName, lpNewFileName);
	
	if(g_Filter.FilterCopyFile((char *)lpExistingFileName, (char *)lpNewFileName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}
	
	return CopyFileA(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL WINAPI myCreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	char pname[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpPathName, -1, pname, 128,NULL,NULL); 
	pname[len] =0;
	WriteLog("CreateDirectoryW:path=%s", pname);

	if(g_Filter.FilterCreateDir(pname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return CreateDirectoryW(lpPathName, lpSecurityAttributes);
}

BOOL WINAPI myCreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	WriteLog("CreateDirectoryA:path=%s", lpPathName);
	if(g_Filter.FilterCreateDir((char *)lpPathName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}
	return CreateDirectoryA(lpPathName, lpSecurityAttributes);
}

BOOL WINAPI myCreateDirectoryExW(LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	char pname1[128], pname2[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpTemplateDirectory, -1, pname1, 128,NULL,NULL); 
	pname1[len] =0;
	len =WideCharToMultiByte( CP_ACP, 0, lpNewDirectory, -1, pname2, 128,NULL,NULL); 
	pname2[len] =0;
	
	WriteLog("CreateDirectoryExW:pathtemp=%s, path=%s", pname1, pname2);

	if(g_Filter.FilterCreateDir(pname1))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	if(g_Filter.FilterCreateDir(pname2))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return CreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);
}

BOOL WINAPI myCreateDirectoryExA(LPCSTR lpTemplateDirectory, LPCSTR lpNewDirectory,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	WriteLog("CreateDirectoryExA:pathtemp=%s, path=%s", lpTemplateDirectory, lpNewDirectory);

	if(g_Filter.FilterCreateDir((char *)lpTemplateDirectory))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}
	
	if(g_Filter.FilterCreateDir((char *)lpNewDirectory))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return CreateDirectoryExA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);
}

HANDLE WINAPI myFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
	HANDLE h =FindFirstFileW(lpFileName, lpFindFileData);
	if(h)
	{
		char fname[128];
		int len =WideCharToMultiByte( CP_ACP, 0, lpFindFileData->cFileName, -1, fname, 128,NULL,NULL); 
		fname[len] =0;

		WriteLog("FindFirstFileW:%s", fname);
		
		if(strstr(fname, "Debug"))//g_Filter.FilterOpenFile(fname))
		{
			WriteLog("okkkkkkkkkkkkkkkkkkkkkk");
			//SetLastError(ERROR_NOT_AUTHENTICATED);
			//return NULL;
			memset(lpFindFileData, 0, sizeof(*lpFindFileData));
			if(!FindNextFileW(h, lpFindFileData))
			{
				FindClose(h);
				SetLastError(ERROR_NOT_AUTHENTICATED);
				return h;
			}
		}
	}
	return h; 
}

HANDLE WINAPI myFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
	WriteLog("FindFirstFileA:%s", lpFileName);
	/*if(g_Filter.FilterFindFile(lpFileName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}*/
	return FindFirstFileA(lpFileName, lpFindFileData);
}
//隐藏文件或目录
HANDLE WINAPI myFindFirstFileExW(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPWIN32_FIND_DATAW lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	HANDLE h=FindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
	if(h)
	{
		char fname[128];
		int len =WideCharToMultiByte( CP_ACP, 0, lpFindFileData->cFileName, -1, fname, 128,NULL,NULL); 
		fname[len] =0;

		WriteLog("FindFirstFileW:%s", fname);
		
		if(g_Filter.FilterOpenFile(fname))
		{
			//WriteLog("okkkkkkkkkkkkkkkkkkkkkk");
			//SetLastError(ERROR_NOT_AUTHENTICATED);
			//return NULL;
			memset(lpFindFileData, 0, sizeof(*lpFindFileData));
			if(!FindNextFileW(h, lpFindFileData))
			{
				FindClose(h);
				SetLastError(ERROR_NOT_AUTHENTICATED);
				return h;
			}
		}
	}
	return h; 
}
//隐藏文件或目录
BOOL WINAPI myFindNextFileW(HANDLE h, LPWIN32_FIND_DATAW lpFindFileData)
{
	//WriteLog("FindNextFileW");
	BOOL f =FindNextFileW(h, lpFindFileData);
	if(f)
	{
		char fname[128];
		int len =WideCharToMultiByte( CP_ACP, 0, lpFindFileData->cFileName, -1, fname, 128,NULL,NULL); 
		fname[len] =0;

		WriteLog("FindNextFileW:%s", fname);
		
		if(g_Filter.FilterOpenFile(fname))  // 如果发现了此文件或目录，就跳过返回下一个查找结果
		{
			//WriteLog("ok2 kkkkkkkkkkkkkkkkkkkkk");
			//SetLastError(ERROR_NOT_AUTHENTICATED);
			//return NULL;
			return FindNextFileW(h, lpFindFileData);
		}
	}
	return f; 
}

BOOL WINAPI myFindNextFileA(HANDLE h, LPWIN32_FIND_DATAA lpFindFileData)
{
	BOOL f =FindNextFileA(h, lpFindFileData);
	if(f)
	{
		WriteLog("FindNextFileA:%s", lpFindFileData->cFileName);
		
		if(g_Filter.FilterOpenFile(lpFindFileData->cFileName))
		{
			//WriteLog("ok2 kkkkkkkkkkkkkkkkkkkkk");
			//SetLastError(ERROR_NOT_AUTHENTICATED);
			//return NULL;
			return FindNextFileA(h, lpFindFileData);
		}
	}
	return f; 
}

BOOL WINAPI myMoveFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName)
{
	char fname[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpExistingFileName, -1, fname, 128,NULL,NULL); 
	fname[len] =0;
	char fname1[128];
	len =WideCharToMultiByte( CP_ACP, 0, lpNewFileName, -1, fname1, 128,NULL,NULL); 
	fname1[len] =0;

	WriteLog("MoveFileW:%s,%s", fname, fname1);

	if(g_Filter.FilterMoveFile(fname, fname1))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return MoveFileW(lpExistingFileName, lpNewFileName);
}

BOOL WINAPI myMoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName)
{
	WriteLog("MoveFileA:%s,%s", lpExistingFileName, lpNewFileName);

	if(g_Filter.FilterMoveFile((char *)lpExistingFileName, (char *)lpNewFileName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return MoveFileA(lpExistingFileName, lpNewFileName);
}

BOOL WINAPI myMoveFileExW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags)
{
	char fname[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpExistingFileName, -1, fname, 128,NULL,NULL); 
	fname[len] =0;
	char fname1[128];
	len =WideCharToMultiByte( CP_ACP, 0, lpNewFileName, -1, fname1, 128,NULL,NULL); 
	fname1[len] =0;

	WriteLog("MoveFileExW:%s,%s", fname, fname1);

	if(g_Filter.FilterMoveFile(fname, fname1))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);
}

BOOL WINAPI myMoveFileExA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, DWORD dwFlags)
{
	WriteLog("MoveFileExA:%s,%s", lpExistingFileName, lpNewFileName);

	if(g_Filter.FilterMoveFile((char *)lpExistingFileName, (char *)lpNewFileName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return MoveFileExA(lpExistingFileName, lpNewFileName, dwFlags);
}

BOOL WINAPI myReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	//WriteLog("ReadFile:handle=%x", hFile);

	char fname[200];
	GetFileNameByHandle(hFile, fname);
	if(g_Filter.FilterReadFile(fname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return NULL;
	}

	return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

BOOL WINAPI myReadFileEx(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
		LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	WriteLog("ReadFileEx");
	char fname[200];
	GetFileNameByHandle(hFile, fname);
	if(g_Filter.FilterReadFile(fname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}
	return ReadFileEx(hFile, lpBuffer, nNumberOfBytesToRead,
		lpOverlapped, lpCompletionRoutine);
}

BOOL WINAPI mySetCurrentDirectoryW(LPCWSTR lpPathName)
{
	char pname[128];
	int len =WideCharToMultiByte( CP_ACP, 0, lpPathName, -1, pname, 128,NULL,NULL); 
	pname[len] =0;
	
	WriteLog("SetCurrentDirectoryW:%s", pname);
	
	if(strstr(pname, "Debug"))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}

	if(g_Filter.FilterChDir(pname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}

	return SetCurrentDirectoryW(lpPathName);
}

BOOL WINAPI mySetCurrentDirectoryA(LPCSTR lpPathName)
{
	WriteLog("SetCurrentDirectoryA:%s", lpPathName);

	if(g_Filter.FilterChDir((char *)lpPathName))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}

	return SetCurrentDirectoryA(lpPathName);
}

BOOL WINAPI myWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
		LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	//WriteLog("WriteFile");

	char fname[200];
	GetFileNameByHandle(hFile, fname);
	if(g_Filter.FilterWriteFile(fname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}

	return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten,
		lpOverlapped);
}

BOOL WINAPI myWriteFileEx(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
		LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	WriteLog("WriteFileEx");

	char fname[200];
	GetFileNameByHandle(hFile, fname);
	if(g_Filter.FilterWriteFile(fname))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}

	return WriteFileEx(hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);
}

// 过滤执行文件
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

	//WriteLog("CreateProcessW :cmd=%s", cmd);

	if(g_Filter.FilterExecute(cmd))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}

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
	if(g_Filter.FilterExecute(lpCommandLine))
	{
		SetLastError(ERROR_NOT_AUTHENTICATED);
		return false;
	}

	BOOL ifsuccess = CreateProcessA(lpApplicationName,
		lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	DWORD err =GetLastError();
	WriteLog("CreateProcessA %s", lpCommandLine);
	SetLastError(err);
	return (DWORD)ifsuccess;
}

DWORD WINAPI myGetFileAttributesW(LPCWSTR pfile)
{
	char fname[128];
	int len=WideCharToMultiByte(CP_ACP, 0, pfile, -1, fname, sizeof(fname), NULL, NULL);
	fname[len] =0;
	WriteLog("myGetFileAttributesW:%s",fname);

	return GetFileAttributesW(pfile);
}

DWORD WINAPI myGetFileAttributesA(LPCSTR pfile)
{
	WriteLog("myGetFileAttributesA:%s",pfile);

	return GetFileAttributesA(pfile);
}

MYAPIINFO myapi_info[] =
{
	{"KERNEL32.DLL", "CreateFileA", 7, "myCreateFileA"},
	{"KERNEL32.DLL", "CreateFileW", 7, "myCreateFileW"},
	{"KERNEL32.DLL", "ReadFile", 5, "myReadFile"},
	{"KERNEL32.DLL", "ReadFileEx", 6, "myReadFileEx"},
	{"KERNEL32.DLL", "WriteFile", 5, "myWriteFile"},
	{"KERNEL32.DLL", "WriteFileEx", 5, "myWriteFileEx"},
	{"KERNEL32.DLL", "MoveFileA", 2, "myMoveFileA"},
	{"KERNEL32.DLL", "MoveFileW", 2, "myMoveFileW"},
	{"KERNEL32.DLL", "MoveFileExA", 3, "myMoveFileExA"},
	{"KERNEL32.DLL", "MoveFileExW", 3, "myMoveFileExW"},
	{"KERNEL32.DLL", "DeleteFileA", 1, "myDeleteFileA"},
	{"KERNEL32.DLL", "DeleteFileW", 1, "myDeleteFileW"},
	{"KERNEL32.DLL", "CopyFileA", 3, "myCopyFileA"},
	{"KERNEL32.DLL", "CopyFileW", 3, "myCopyFileW"},
	{"KERNEL32.DLL", "FindFirstFileA", 2, "myFindFirstFileA"},
	{"KERNEL32.DLL", "FindFirstFileW", 2, "myFindFirstFileW"},
	{"KERNEL32.DLL", "FindNextFileW", 2, "myFindNextFileW"},
	{"KERNEL32.DLL", "CreateDirectoryExA", 3, "myCreateDirectoryExA"},
	{"KERNEL32.DLL", "CreateDirectoryExW", 3, "myCreateDirectoryExW"},
	{"KERNEL32.DLL", "CreateDirectoryA", 2, "myCreateDirectoryA"},
	{"KERNEL32.DLL", "CreateDirectoryW", 2, "myCreateDirectoryW"},
	{"KERNEL32.DLL", "SetCurrentDirectoryA", 1, "mySetCurrentDirectoryA"},
	{"KERNEL32.DLL", "SetCurrentDirectoryW", 1, "mySetCurrentDirectoryW"},
	{"KERNEL32.DLL", "CreateProcessW", 10, "myCreateProcessW"},
	{"KERNEL32.DLL", "CreateProcessA", 10, "myCreateProcessA"},
	{"KERNEL32.DLL", "GetFileAttributesW", 1, "myGetFileAttributesW"},
	{"KERNEL32.DLL", "GetFileAttributesA", 1, "myGetFileAttributesA"},
	{"KERNEL32.DLL", "FindFirstFileExW", 6, "myFindFirstFileExW"},
	
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
