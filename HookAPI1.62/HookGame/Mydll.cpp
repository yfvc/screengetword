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
#include "../dll/mydll.h"
//#include <nb30.h>
#include <ras.h>
#include "util.h"

#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

HINSTANCE g_hInstance;

// 如果是win9x，不能使用fopen函数
int WriteBinData(char *function, char *buf, int len)
{
	char mod_name[100];
	char fname[128];

	if(len <=0) return 0;

	GetFileName(mod_name);
	wsprintf(fname, "c:\\%s.log", mod_name);
	HANDLE hFile;

	if((hFile =CreateFile(fname, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0)
	{
		WriteLog("open file %s failed", fname);
		return -1;
	}
	_llseek((HFILE)hFile, 0, SEEK_END);
	char temp[2048];
	wsprintf(temp, "\r\n(%s,len=%d) ", function, len);
	DWORD dw;
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	for(int i =0; i<len; i++)
	{
		if(isgraph(buf[i]&0x00FF))
			wsprintf(temp, "%c", buf[i]&0x00FF);
		else
		    wsprintf(temp, "(%02x)", buf[i]&0x00FF);
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	}
	_lclose((HFILE)hFile);

	return 0;
}

void peek_message(void)
{
  MSG msg;

  if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void SetKBEvent(BYTE key, BYTE scancode, BOOL f_down)
{
	// 0x1D: control, 0x2A: shift
	keybd_event(key, scancode, f_down?KEYEVENTF_EXTENDEDKEY:KEYEVENTF_KEYUP, 0);
}

int g_cur_x =0, g_width =0, g_cur_min_y =40;
int g_cur_event =0, f_timer_killed =true, g_TimerID =0;
int g_win_width =0, g_win_height =0, g_min_y =1000, g_min_yx =0;
int g_min_y2 =0, g_min_yx2 =0;

void CALLBACK FuncKillTimer(HWND hWnd, UINT umsg, UINT idEvent, DWORD dwTime)
{
	KillTimer(NULL, g_TimerID);
	if(g_cur_event ==VK_RIGHT)
		SetKBEvent(VK_RIGHT, VK_RIGHT, false);
	else SetKBEvent(VK_LEFT, VK_LEFT, false);
	f_timer_killed =true;
}

int f_kb_space =0;
void CALLBACK FuncKBSpace(HWND hWnd, UINT umsg, UINT idEvent, DWORD dwTime)
{
	MessageBeep(0);
	PostMessage(GetFocus(), WM_KEYDOWN, VK_SPACE, 0L);
}

BOOL WINAPI myBitBlt(HDC hdcDest, int x, int y, int nWidth, int nHeight, HDC hdcSrc,
				   int xSrc, int ySrc, DWORD dwRop )
{
	char fname[256];
	GetModuleFileName(NULL, fname, sizeof(fname));
	//if(strstr(fname, "ship.exe"))
	{
		WriteLog("BitBlt:thread_id: %d, x=%d, y=%d, width=%d, height=%d",
			GetCurrentThreadId(), x, y, nWidth, nHeight);
		
		/*if(g_win_width ==0)
		{
			RECT rc;
			GetClientRect(GetFocus(), &rc);
			g_win_width =rc.right;
			g_win_height =rc.bottom;
		}
		if(y ==30)
		{
			if(g_cur_x -x > nWidth || x -g_cur_x-x > nWidth)
			{
				SetKBEvent(VK_RIGHT, VK_RIGHT, false);
				SetKBEvent(VK_LEFT, VK_LEFT, false);
			}
			g_cur_x =x;
			if(g_width ==0)
				g_width =nWidth;
		}*/

		/*if(f_kb_space ==0)
		{
			f_kb_space =1;
			SetTimer(NULL, 1, 500, FuncKBSpace);
		}*/

		/*if(g_min_yx2 < g_cur_x - g_width || g_min_yx2 > g_cur_x +g_width*2)
			PostMessage(GetFocus(), WM_KEYDOWN, VK_SPACE, 0L);
		if((y>30 && y<100 && x>g_cur_x-50 && x < (g_cur_x+g_width)+50 && nWidth>10 && nWidth <20) && f_timer_killed)
		{
			//WriteLog("x:%d, g_cur_x:%d, g_nWidth:%d, nWidth:%d", x, g_cur_x,
			//	g_width, nWidth);
			//if(y <=g_min_y)
			{
			if(g_cur_x < 10)
			{
				//if(g_cur_event !=VK_RIGHT)
				{
					SetKBEvent(VK_RIGHT, VK_RIGHT, true);
				}
				g_cur_event =VK_RIGHT;
			}
			else if(g_cur_x >g_win_width-g_width-10 && x < g_cur_x - 80)
			{
				//if(g_cur_event !=VK_LEFT)
				{
					SetKBEvent(VK_LEFT, VK_LEFT, true);
				}
				g_cur_event =VK_LEFT;
			}
			else if(x > g_cur_x -10 && x < g_cur_x+g_width/2)
			{
				//if(g_cur_event !=VK_RIGHT)
				{
					SetKBEvent(VK_RIGHT, VK_RIGHT, true);
				}
				g_cur_event =VK_RIGHT;
			}
			else if (x >= g_cur_x+g_width/2 && x < g_cur_x + g_width +10)
			{
				//if(g_cur_event !=VK_LEFT)
				{
					SetKBEvent(VK_LEFT, VK_LEFT, true);
				}
				g_cur_event =VK_LEFT;
			}
			wsprintf(fname, "%d,%d", x, y);
			TextOut(hdcDest, 0, 0, fname, strlen(fname));
			//for(int i =0; i<100000000; i++)
			//	peek_message();
			//Sleep(0);
			if(y < g_min_y)
			{
				g_min_y =y+5;
				g_min_yx =x;
			}*/
			/*if(g_min_y <35)
				g_min_y =1000;*/
			/*g_TimerID =SetTimer(NULL, 0, 800, FuncKillTimer);
			f_timer_killed =false;
			}
		}*/
		/*else if(nWidth >40)
		{
			if(y <500)
				g_min_yx2 =x;
		}*/
	}

	return BitBlt(hdcDest, x, y, nWidth, nHeight, hdcSrc, xSrc, ySrc, dwRop);
}

// 地址小的放前面
MYAPIINFO myapi_info[] =
{
	{"GDI32.DLL", "BitBlt", 9, "myBitBlt"},
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

