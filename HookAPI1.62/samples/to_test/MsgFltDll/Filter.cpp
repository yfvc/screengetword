#include "stdafx.h"
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "Filter.h"
#include "const.h"

char g_dll_path[128];

CMsgFilter::CMsgFilter()
{
	m_pFilterInfo =NULL;
	m_nFilterInfoCount =0;
	
	FILE *fp;

	if(GetProfileString("HookAPI", "dll_path", "", g_dll_path, sizeof(g_dll_path)) ==0)
		return;
	char init_file[128];

	wsprintf(init_file, "%s\\filtermsg.ini", g_dll_path);
	//WriteLog("FilterInit:init_file=%s", init_file);
	if((fp =fopen(init_file, "r")) ==NULL)
	{
		WriteLog("open init file %s failed!", init_file);
		return;
	}
	char buf[300], *p, caption[40], msg[30];
	while(!feof(fp) && fgets(buf, sizeof(buf), fp))
	{
		if(strlen(buf) <2 || buf[0] =='#') continue;
		if(buf[strlen(buf)-1] =='\r' || buf[strlen(buf)-1] =='\n')
			buf[strlen(buf)-1] =0;
		if(buf[strlen(buf)-2] =='\r' || buf[strlen(buf)-2] =='\n')
			buf[strlen(buf)-2] =0;
		
		p =strtok(buf, ",");
		if(!p) continue;
		if(strlen(p) >= sizeof(msg)) continue;
		strcpy(msg, p);
		
		caption[0] =0;
		p =strtok(NULL, ",");
		if(p)
		{
			if(strlen(p) >=sizeof(caption)) continue;
			strcpy(caption, p);
			strupr(caption);
		}
		if(m_pFilterInfo)
			m_pFilterInfo =(FILTER_INFO *)realloc(m_pFilterInfo, (m_nFilterInfoCount+1)*sizeof(FILTER_INFO));
		else
			m_pFilterInfo =(FILTER_INFO *)malloc((m_nFilterInfoCount+1)*sizeof(FILTER_INFO));
		memset(&m_pFilterInfo[m_nFilterInfoCount], 0, sizeof(FILTER_INFO));
		int id =0;
		if(msg[0] >='0' && msg[0] <='9')
			id =atoi(msg);
		else
		{
			for(int i=0; i<g_msg_count; i++)
			{
				if(!strcmpi(g_msg_info[i].name, msg))
				{
					id =g_msg_info[i].id;
					break;
				}
			}
			if(i ==g_msg_count)
				WriteLog("Can not found message %s", msg);
		}
		m_pFilterInfo[m_nFilterInfoCount].id =id;
		strcpy(m_pFilterInfo[m_nFilterInfoCount].win_caption, caption);
		//WriteLog("filter:%d,%d,%s.", m_nFilterInfoCount, id, caption);

		m_nFilterInfoCount++;
	}
	//WriteLog("filercount=%d", m_nFilterInfoCount);
}

CMsgFilter::~CMsgFilter()
{
	if(m_pFilterInfo)
		free(m_pFilterInfo);
}

int CMsgFilter::Filter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char txt[512];

	//if(msg >=(UINT)g_msg_count)
	//	return false;
	txt[0] =0;
	GetWindowText(hWnd, txt, sizeof(txt));
	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		//WriteLog("*************msg=%d, id=%d", msg, m_pFilterInfo[i].id);
		if(m_pFilterInfo[i].win_caption[0] && strcmpi(m_pFilterInfo[i].win_caption, txt))
			continue;
		if(msg ==m_pFilterInfo[i].id)
		{
			/*TCITEMW *pitem =(TCITEMW *)lParam;
			char temp[100];
			WideCharToMultiByte( CP_ACP, 0, pitem->pszText, -1, temp, 128,NULL,NULL); 
			*/
			WriteLog("===============Filter %d ok, wParam=0x%x, lParam=0x%x", msg, wParam, lParam);
			return true;
		}
	}
	return false;
}

CMsgFilter::FilterSendMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//WriteLog("SendMessage");
	return Filter(hWnd, msg, wParam, lParam);
}

CMsgFilter::FilterPostMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//WriteLog("PostMessage");
	return Filter(hWnd, msg, wParam, lParam);
}

CMsgFilter::FilterCallWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//WriteLog("CallWindowProc");
	return Filter(hWnd, msg, wParam, lParam);
}

CMsgFilter::FilterDispatchMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//WriteLog("DispatchMessage");
	return Filter(hWnd, msg, wParam, lParam);
}
