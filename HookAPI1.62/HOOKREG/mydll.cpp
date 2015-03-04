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

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

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

char *GetRootKey(HKEY hKey)
{
	if(hKey == HKEY_CLASSES_ROOT)
		return "HKEY_CLASSES_ROOT";
	else if(hKey == HKEY_CURRENT_CONFIG)
		return "KEY_CURRENT_CONFIG";
	else if(hKey ==HKEY_CURRENT_USER)
		return "HKEY_CURRENT_USER";
	else if(hKey == HKEY_LOCAL_MACHINE)
		return "HKEY_LOCAL_MACHINE";
	else if(hKey == HKEY_USERS)
		return "HKEY_USERS";
	else if(hKey == HKEY_PERFORMANCE_DATA)
		return "HKEY_PERFORMANCE_DATA";
	else
		return "未知根路径";
}

DWORD WINAPI myRegOpenKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));

	int ret =RegOpenKeyA(hKey, lpSubKey, phkResult);
	int err =GetLastError();
	WriteLog("%s, myRegOpenKeyA:hKey=%s,SubKey=%s, hKeyResult=%x", temp, GetRootKey(hKey), lpSubKey, *phkResult);
	SetLastError(err);

	return ret;
}

DWORD WINAPI myRegOpenKeyW(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult)
{
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	char subkey[200];
	int len =WideCharToMultiByte( CP_ACP, 0, lpSubKey, -1, subkey, sizeof(subkey),NULL,NULL); 
	subkey[len] =0;
	WriteLog("%s, myRegOpenKeyW:hKey=%s,SubKey=%s", temp, GetRootKey(hKey), subkey);

	return (DWORD)RegOpenKeyW(hKey, lpSubKey, phkResult);
}

DWORD WINAPI myRegQueryValueA(HKEY hKey, LPCSTR lpSubKey, LPSTR lpValue, PLONG lpcbValue)
{
	int err;
	LONG ret =RegQueryValueA(hKey, lpSubKey, lpValue, lpcbValue);
	err =GetLastError();
	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	if(strcmpi(lpSubKey, "UseCount") ==0)
	{
		*(int *)lpValue =0;
	}
	WriteLog("%s, myRegQueryValueA:hKey=%x,SubKey=%s", temp, hKey, lpSubKey);
	SetLastError(err);

	return (DWORD)ret;
}

DWORD WINAPI myRegQueryValueW(HKEY hKey,LPCWSTR lpSubKey, LPWSTR lpValue, PLONG lpcbValue)
{
	LONG ret =RegQueryValueW(hKey, lpSubKey, lpValue, lpcbValue);
	int err =GetLastError();

	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	char subkey[200];
	int len =WideCharToMultiByte( CP_ACP, 0, lpSubKey, -1, subkey, sizeof(subkey),NULL,NULL); 
	subkey[len] =0;
	WriteLog("%s, myRegQueryValueW:hKey=%s,SubKey=%s", temp, GetRootKey(hKey), subkey);
	SetLastError(err);
	
	return (DWORD)ret;
}

DWORD WINAPI myRegQueryValueExA(HKEY hKey, LPSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	LONG ret =RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
	int err =GetLastError();

	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	WriteLog("%s, myRegQueryValueExA:hKey=%s,ValueName=%s", temp, GetRootKey(hKey), lpValueName);
	SetLastError(err);
	
	return (DWORD)ret;
}

DWORD WINAPI myRegQueryValueExW(HKEY hKey, LPWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	LONG ret =RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
	int err =GetLastError();

	char temp[200];
	GetModuleFileName(NULL, temp, sizeof(temp));
	char value[200];
	int len =WideCharToMultiByte( CP_ACP, 0, lpValueName, -1, value, sizeof(value),NULL,NULL); 
	value[len] =0;
	WriteLog("%s, myRegQueryValueExW:hKey=%s,ValueName=%s", temp, GetRootKey(hKey), value);
	SetLastError(err);
	
	return (DWORD)ret;
}

MYAPIINFO myapi_info[] =
{
	{"ADVAPI32.DLL", "RegOpenKeyW", 3, "myRegOpenKeyW"},
	{"ADVAPI32.DLL", "RegQueryValueW", 4, "myRegQueryValueW"},
	{"ADVAPI32.DLL", "RegOpenKeyA", 3, "myRegOpenKeyA"},
	{"ADVAPI32.DLL", "RegQueryValueA", 4, "myRegQueryValueA"},
	{"ADVAPI32.DLL", "RegQueryValueExW", 6, "myRegQueryValueExW"},
	{"ADVAPI32.DLL", "RegQueryValueExA", 6, "myRegQueryValueExA"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
