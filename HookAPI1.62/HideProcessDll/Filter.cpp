#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "Filter.h"

char g_dll_path[128];

CFilter::CFilter()
{
	m_pFilterInfo =NULL;
	m_nFilterInfoCount =0;
	
	FILE *fp;

	if(GetProfileString("HookAPI", "dll_path", "", g_dll_path, sizeof(g_dll_path)) ==0)
		return;
	char init_file[128];

	wsprintf(init_file, "%s\\filterhp.ini", g_dll_path);
	//WriteLog("FilterInit:init_file=%s", init_file);
	if((fp =fopen(init_file, "r")) ==NULL)
	{
		WriteLog("CFilter: can not found init file %s", init_file);
		return;
	}
	char buf[256];
	while(!feof(fp) && fgets(buf, sizeof(buf), fp))
	{
		if(strlen(buf) <3 || buf[0] =='#') continue;
		if(buf[strlen(buf)-1] =='\r' || buf[strlen(buf)-1] =='\n')
			buf[strlen(buf)-1] =0;
		if(buf[strlen(buf)-2] =='\r' || buf[strlen(buf)-2] =='\n')
			buf[strlen(buf)-2] =0;
		
		if(m_pFilterInfo)
			m_pFilterInfo =(FILTER_INFO *)realloc(m_pFilterInfo, (m_nFilterInfoCount+1)*sizeof(FILTER_INFO));
		else
			m_pFilterInfo =(FILTER_INFO *)malloc((m_nFilterInfoCount+1)*sizeof(FILTER_INFO));
		strcpy(m_pFilterInfo[m_nFilterInfoCount].app, buf);
		//WriteLog("init:%s", buf);
		m_nFilterInfoCount++;
	}
}

CFilter::~CFilter()
{
	if(m_pFilterInfo)
		free(m_pFilterInfo);
}

int CFilter::FilterName(char *name)
{
	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		//WriteLog("i=%d, name=%s, app=%s", i, name, m_pFilterInfo[i].app);
		if(!strcmpi(name, m_pFilterInfo[i].app))
			return true;
	}
	return false;
}
