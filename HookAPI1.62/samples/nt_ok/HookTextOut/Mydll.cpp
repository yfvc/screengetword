// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// 编译提示：如果是Win9x系统，需要预定义WIN95 //
// ------------------------------------- //

#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mydll.h"
#include "util.h"

#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

BOOL WINAPI myTextOutW(HDC hDC, int x, int y, LPCWSTR str, int len)
{
	WriteLog("myTextOutW");
	if(len >0)
	{
		char *pstr =(char *)LocalAlloc(LMEM_FIXED, len*2+1);
		int len2 =WideCharToMultiByte(CP_ACP, 0, str, len, pstr, len*2, NULL, NULL);
		pstr[len2] =0;

		WriteLog("myTextOutW, x:%d, y:%d, str:%s, len:%d", x, y, pstr, len);
		LocalFree(pstr);
	}

	return TextOutW(hDC, x, y, str, len);
}

BOOL WINAPI myTextOutA(HDC hDC, int x, int y, LPCSTR str, int len)
{
#ifdef WIN95
	unsigned char cl_val;
	_asm
	{
		Mov [cl_val], CL
	}

	WriteLog("myTextOutA, cl:%d", cl_val);
	if(cl_val ==50) return myTextOutW(hDC, x, y, (LPCWSTR)str, len);
#endif

	if(len >0)
	{
		char *pstr =(char *)LocalAlloc(LMEM_FIXED, len+1);
		strncpy(pstr, str, len);
		pstr[len] =0;
		WriteLog("myTextOutA, x:%d, y:%d, str:%s, len:%d", x, y, pstr, len);
		LocalFree(pstr);
	}
	return TextOutA(hDC, x, y, str, len);
}

BOOL WINAPI myExtTextOutA(HDC hDC, int x, int y, UINT options, const RECT *lprc, LPCSTR str, UINT len, CONST INT *lpDx)
{
	WriteLog("myExtTextOutA, len:%d", len);
	if(len >0)
	{
		char *pstr =(char *)LocalAlloc(LMEM_FIXED, len+1);
		strncpy(pstr, str, len);
		pstr[len] =0;
		WriteLog("myExtTextOutA, x:%d, y:%d, str:%s, len:%d", x, y, pstr, len);
		LocalFree(pstr);
	}
	return ExtTextOutA(hDC, x, y, options, lprc, str, len, lpDx);
}

BOOL WINAPI myExtTextOutW(HDC hDC, int x, int y, UINT options, const RECT *lprc, LPCWSTR str, UINT len, CONST INT *lpDx)
{
	WriteLog("myExtTextOutW, len:%d", len);
	if(len >0)
	{
		char *pstr =(char *)LocalAlloc(LMEM_FIXED, len*2+1);
		int len2 =WideCharToMultiByte(CP_ACP, 0, str, len, pstr, len*2, NULL, NULL);
		pstr[len2] =0;

		WriteLog("myExtTextOutW, x:%d, y:%d, str:%s, len:%d", x, y, pstr, len);
		LocalFree(pstr);
	}
	return ExtTextOutW(hDC, x, y, options, lprc, str, len, lpDx);
}

// 地址小的放前面
MYAPIINFO myapi_info[] =
{
#ifdef WIN95
	{"GDI32.DLL", "TextOutA", 5, "myTextOutA", 2},
	{"GDI32.DLL", "ExtTextOutW", 8, "myExtTextOutW", 0, "myExtTextOutA"},
#else
	{"GDI32.DLL", "TextOutW", 5, "myTextOutW"},
	{"GDI32.DLL", "TextOutA", 5, "myTextOutA"},
	{"GDI32.DLL", "ExtTextOutW", 8, "myExtTextOutW"},
#endif
	{"GDI32.DLL", "ExtTextOutA", 8, "myExtTextOutA"},
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

