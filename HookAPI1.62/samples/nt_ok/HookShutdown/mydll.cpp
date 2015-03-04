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

BOOL WINAPI myExitWindowsEx(UINT uFlags,DWORD dwReserved)
{
   char temp[200];
   GetModuleFileName(NULL, temp, sizeof(temp));
   WriteLog("%s, myExitWindowsEx:uFlags=%u", temp, uFlags);
   if(uFlags & EWX_REBOOT)
   {
     return  ExitWindowsEx(uFlags,dwReserved);
   }
   else
     return FALSE;
}

MYAPIINFO myapi_info[] =
{
	{"USER32.DLL", "ExitWindowsEx", 2, "myExitWindowsEx"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
