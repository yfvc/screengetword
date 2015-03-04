// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// 编译提示：如果是Win9x系统，需要预定义WIN95 //
// ------------------------------------- //

#include "stdafx.h"
#include <winsock.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <lmcons.h>
//#include <lmalert.h>
#include "mydll.h"
//#include <nb30.h>
#include <ras.h>
#include "util.h"

#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

HINSTANCE g_hInstance;

HANDLE WINAPI myLoadLibraryW(LPCWSTR plib)
{
	char lib_name[128];
	int len =WideCharToMultiByte( CP_ACP, 0, plib, -1, lib_name, 128,NULL,NULL); 
	lib_name[len] =0;

	HANDLE hlib = LoadLibraryW(plib);
	
	//DWORD err =GetLastError();
	WriteLog("LoadLibraryW: plib=%s, hlib=%x", lib_name, hlib);
	//SetLastError(err);

	return hlib;
}

HANDLE WINAPI myLoadLibraryA(LPCSTR plib)
{
	HANDLE hlib = LoadLibraryA(plib);
	//DWORD err =GetLastError();
	WriteLog("LoadLibraryA: plib=%s, hlib=%x", plib, hlib);
	//SetLastError(err);
	return hlib;
}

FARPROC WINAPI myGetProcAddress(HMODULE hlib, LPCSTR func_name)
{
	FARPROC proc =GetProcAddress(hlib, func_name);
	char temp[256];

	DWORD err =GetLastError();
	char mod_name[256];
	GetModuleFileName(hlib, mod_name, sizeof(mod_name));
	wsprintf(temp, "GetProcAddress:mod=%s, func_name=%s", mod_name, func_name);
	
	WriteLog(temp);

	SetLastError(err);

	return proc;
}

MYAPIINFO myapi_info[] =
{
	{"KERNEL32.DLL", "GetProcAddress", 2, "myGetProcAddress"},
	{"KERNEL32.DLL", "LoadLibraryW", 1, "myLoadLibraryW"},
	{"KERNEL32.DLL", "LoadLibraryA", 1, "myLoadLibraryA"},
	{NULL}
};

// 下列内容请不要修改
MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call =DLL_PROCESS_ATTACH)
	{
		//GetProfileString("HookAPI", "dll_path", "", g_szDllPath, sizeof(g_szDllPath));
	}

	return TRUE;
}

