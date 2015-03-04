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
#include "filter.h"

HINSTANCE g_hInstance =NULL;
CFileFilter g_Filter;

HANDLE WINAPI myFindFirstFile(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
	HANDLE h =FindFirstFile(lpFileName, lpFindFileData);
	if(h)
	{
		if(g_Filter.FilterName(lpFindFileData->cFileName, FILTER_FIND) || strstr(lpFindFileData->cFileName, "myabc123"))
		{
			memset(lpFindFileData, 0, sizeof(*lpFindFileData));
			if(!FindNextFile(h, lpFindFileData))
			{
				FindClose(h);
				//SetLastError(-1);
				return h;
			}
		}
	}
	return h; 
}

BOOL WINAPI myFindNextFile(HANDLE h, LPWIN32_FIND_DATA lpFindFileData)
{
	BOOL f =FindNextFile(h, lpFindFileData);
	if(f)
	{
		if(g_Filter.FilterName(lpFindFileData->cFileName, FILTER_FIND) || strstr(lpFindFileData->cFileName, "myabc123"))
			return FindNextFile(h, lpFindFileData);
	}
	return f; 
}

MYAPIINFO myapi_info[] =
{
	{"KERNEL32.DLL", "FindFirstFile(LPCSTR, LPWIN32_FIND_DATA)", "myFindFirstFile"},
	{"KERNEL32.DLL", "FindNextFile(HANDLE, LPWIN32_FIND_DATAW)", "myFindNextFile"},
	
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance =(HINSTANCE)hModule;
    return TRUE;
}

