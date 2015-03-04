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
CMsgFilter g_Filter;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance =(HINSTANCE)hModule;

    return TRUE;
}

LRESULT WINAPI mySendMessageA(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterSendMessage(hWnd, msg, wParam, lParam))
		return 0;
	return SendMessageA(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI mySendMessageW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterSendMessage(hWnd, msg, wParam, lParam))
		return 0;
	return SendMessageW(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI mySendDlgItemMessageA(HWND hWnd, int nId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterSendMessage(hWnd, msg, wParam, lParam))
		return 0;
	return SendDlgItemMessageA(hWnd, nId, msg, wParam, lParam);
}

LRESULT WINAPI mySendDlgItemMessageW(HWND hWnd, int nId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterSendMessage(hWnd, msg, wParam, lParam))
		return 0;
	return SendDlgItemMessageW(hWnd, nId, msg, wParam, lParam);
}

LRESULT WINAPI myPostMessageA(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterPostMessage(hWnd, msg, wParam, lParam))
		return 0;
	return PostMessageA(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI myPostMessageW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterPostMessage(hWnd, msg, wParam, lParam))
		return 0;
	return PostMessageW(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI myCallWindowProcA(WNDPROC wndproc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterCallWindowProc(hWnd, msg, wParam, lParam))
		return 0;
	return CallWindowProcA(wndproc, hWnd, msg, wParam, lParam);
}

LRESULT WINAPI myCallWindowProcW(WNDPROC wndproc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_Filter.FilterCallWindowProc(hWnd, msg, wParam, lParam))
		return 0;
	return CallWindowProcW(wndproc, hWnd, msg, wParam, lParam);
}

LONG WINAPI myDispatchMessageA(MSG *lpmsg)
{
	if(g_Filter.FilterDispatchMessage(lpmsg->hwnd, lpmsg->message, lpmsg->wParam, lpmsg->lParam))
		return 0;
	return DispatchMessageA(lpmsg);
}

LONG WINAPI myDispatchMessageW(MSG *lpmsg)
{
	//WriteLog("DispatchMessageW");
	if(g_Filter.FilterDispatchMessage(lpmsg->hwnd, lpmsg->message, lpmsg->wParam, lpmsg->lParam))
		return 0;
	return DispatchMessageW(lpmsg);
}

MYAPIINFO myapi_info[] =
{
	{"USER32.DLL", "CallWindowProcA(WNDPROC, HWND, UINT, WPARAM, LPARAM)", "myCallWindowProcA"},
	{"USER32.DLL", "CallWindowProcW(WNDPROC, HWND, UINT, WPARAM, LPARAM)", "myCallWindowProcW"},
	{"USER32.DLL", "SendMessageA(HWND, UINT, WPARAM, LPARAM)", "mySendMessageA"},
	{"USER32.DLL", "SendMessageW(HWND, UINT, WPARAM, LPARAM)", "mySendMessageW"},
	{"USER32.DLL", "SendDlgItemMessageA(HWND, int, UINT, WPARAM, LPARAM)", "mySendDlgItemMessageA"},
	{"USER32.DLL", "SendDlgItemMessageW(HWND, int, UINT, WPARAM, LPARAM)", "mySendDlgItemMessageW"},
	{"USER32.DLL", "PostMessageA(HWND, UINT, WPARAM, LPARAM)", "myPostMessageA"},
	{"USER32.DLL", "PostMessageW(HWND, UINT, WPARAM, LPARAM)", "myPostMessageW"},
	{"USER32.DLL", "DispatchMessageA(LPMSG)", "myDispatchMessageA"},
	{"USER32.DLL", "DispatchMessageW(LPMSG)", "myDispatchMessageW"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
