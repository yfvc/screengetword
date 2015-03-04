#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "Filter.h"

char g_dll_path[128];

CSocketFilter::CSocketFilter()
{
	m_pFilterInfo = NULL;
	m_hLocal =NULL;

	char dll_path[512];

	if(GetProfileString("HOOKAPI", "dll_path", "", dll_path, sizeof(dll_path)) ==0)
		return;

	char ini_file[256];
	wsprintf(ini_file, "%s\\filter.ini", dll_path);
	WriteLog("FilterInit:ini_file=%s", ini_file);

	HANDLE hFile = CreateFile(ini_file, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	_llseek((HFILE)hFile, 0, FILE_BEGIN);

	BOOL bResult;
	DWORD nBytesToRead=1024;
	DWORD nBytesRead=0;
	char buffer[1024];
	char line[256], *p;
	char type[4], app[2], name[256], action[2];
	int i,j;
	bool bFirst=true;

	m_nFilterInfoCount = 0;
	m_hLocal =LocalAlloc(LMEM_FIXED, sizeof(FILTER_INFO)*FILTER_INFO_STEP);
	m_pFilterInfo = (FILTER_INFO*)LocalLock(m_hLocal);

	int len_read;
	bResult =1;
	while (bResult)
	{
		len_read =0;
		buffer[0] =0;
		while((bResult = ReadFile(hFile, &buffer[len_read], 1, &nBytesRead, NULL)))
		{
			if(buffer[len_read] =='\n') break;
			len_read ++;
		}
		if(nBytesRead ==0) break;

		buffer[len_read] =0;
		if(buffer[0] =='#' || buffer[0] ==0 || buffer[0] =='\r') continue;
		strtok(buffer, "\r");
		strtok(buffer, "\n");
		WriteLog("buffer:%s, bResult:%d", buffer, bResult);
		
		p =strtok(buffer, ",");
		if(!p) continue;
		if(strlen(p) >= sizeof(type)) continue;
		strcpy(type, p);
				
		if (stricmp(type,"dns")!=0)
			continue;

		p =strtok(NULL, ",");
		if(!p) continue;
		if(strlen(p) >=sizeof(name)) continue;
		strcpy(name, p);             // host

		m_pFilterInfo[m_nFilterInfoCount].type = FILTER_DNS;
		//strcpy(m_pFilterInfo[m_nFilterInfoCount].app,app);
		strcpy(m_pFilterInfo[m_nFilterInfoCount].buf,name);
		WriteLog("dns name:%s", name);
		m_nFilterInfoCount++;

		if (m_nFilterInfoCount == FILTER_INFO_STEP)
			break;
	}
	WriteLog("m_nFilterInfoCount =%d", m_nFilterInfoCount);

	_lclose((HFILE)hFile);
}

CSocketFilter::~CSocketFilter()
{
	//if(m_pFilterInfo)
	//	free(m_pFilterInfo);
	if(m_hLocal)
	{
		LocalUnlock(m_hLocal);
		LocalFree(m_hLocal);
	}
}

int CSocketFilter::FilterBind(int port)
{
	char app[128];

	GetFileName(app);
	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_BIND)
			continue;
		if(m_pFilterInfo[i].app[0] =='*' || !strcmpi(m_pFilterInfo[i].app, app))
		{
			if(m_pFilterInfo[i].port ==port)
				return true;
		}
	}
	return false;
}

int CSocketFilter::FilterConnect(char *ip, int port)
{
	char app[128];

	GetFileName(app);
	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_CONNECT)
			continue;
		if(m_pFilterInfo[i].app[0] =='*' || !strcmpi(m_pFilterInfo[i].app, app))
		{
			if(m_pFilterInfo[i].ip[0] =='*' || !strcmpi(m_pFilterInfo[i].ip, ip))
			{
				if(m_pFilterInfo[i].port ==0 || m_pFilterInfo[i].port ==port)
					return true;
			}
		}
	}

	return false;
}

int CSocketFilter::FilterSend(int sd, char *buf, int len)
{
	char app[128];
	char ip[16];

	GetFileName(app);

	int port =GetLocalPortBySocket(sd);
	int port1 =GetRemotePortBySocket(sd);
	if(port1 <1024) port =port1;
	GetRemoteIPBySocket(sd, ip);

	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_SEND)
			continue;
		if(m_pFilterInfo[i].app[0] =='*' || !strcmpi(m_pFilterInfo[i].app, app))
		{
			if(m_pFilterInfo[i].ip[0] =='*' || !strcmpi(m_pFilterInfo[i].ip, ip))
			{
				if(m_pFilterInfo[i].port ==0 || m_pFilterInfo[i].port ==port)
				{
					if(m_pFilterInfo[i].buf[0] ==0) return true;
					mreplace(buf, len, m_pFilterInfo[i].buf, ' ');
				}
			}
		}
	}

	return false;
}

int CSocketFilter::FilterRecv(int sd, char *buf, int len)
{
	char app[128];
	char ip[16];

	GetFileName(app);

	int port =GetLocalPortBySocket(sd);
	int port1 =GetRemotePortBySocket(sd);
	if(port1 <1024) port =port1;
	GetRemoteIPBySocket(sd, ip);
	//WriteLog("FilterInfoCount=%d", m_nFilterInfoCount);
	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		//WriteLog("FilterRecv:type=%d,app=%s,ip=%s,port=%d,buf=%s",
		//	m_pFilterInfo[i].type, m_pFilterInfo[i].app, m_pFilterInfo[i].ip,
		//	m_pFilterInfo[i].port,m_pFilterInfo[i].buf);

		if(m_pFilterInfo[i].type !=FILTER_RECV)
			continue;
		if(m_pFilterInfo[i].app[0] =='*' || !strcmpi(m_pFilterInfo[i].app, app))
		{
			if(m_pFilterInfo[i].ip[0] =='*' || !strcmpi(m_pFilterInfo[i].ip, ip))
			{
				if(m_pFilterInfo[i].port ==0 || m_pFilterInfo[i].port ==port)
				{
					if(m_pFilterInfo[i].buf[0] ==0) return true;
					mreplace(buf, len, m_pFilterInfo[i].buf, ' ');
				}
			}
		}
	}

	return false;
}

int CSocketFilter::FilterDNS(char *host_name)
{
	if(host_name ==NULL) return false;

	//char app[128];

//WriteLog("FilterDNS:host_name:%s", host_name);
	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_DNS)
			continue;
//WriteLog("FilterDNS:host_name:%s,%s", host_name, m_pFilterInfo[i].buf);
		if(!strcmpi(host_name, m_pFilterInfo[i].buf))
			return true;
	}
	
	return false;
}

int CSocketFilter::FilterSendTo(char *ip, int port, char *buf, int len)
{
	char app[128];

	GetFileName(app);

	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_SENDTO)
			continue;
		if(m_pFilterInfo[i].app[0] =='*' || !strcmpi(m_pFilterInfo[i].app, app))
		{
			if(m_pFilterInfo[i].ip[0] =='*' || !strcmpi(m_pFilterInfo[i].ip, ip))
			{
				if(m_pFilterInfo[i].port ==0 || m_pFilterInfo[i].port ==port)
				{
					if(m_pFilterInfo[i].buf[0] ==0) return true;
					mreplace(buf, len, m_pFilterInfo[i].buf, ' ');
				}
			}
		}
	}

	return false;
}

int CSocketFilter::FilterRecvFrom(char *ip, int port, char *buf, int len)
{
	char app[128];

	GetFileName(app);

	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_RECVFROM)
			continue;
		if(m_pFilterInfo[i].app[0] =='*' || !strcmpi(m_pFilterInfo[i].app, app))
		{
			if(m_pFilterInfo[i].ip[0] =='*' || !strcmpi(m_pFilterInfo[i].ip, ip))
			{
				if(m_pFilterInfo[i].port ==0 || m_pFilterInfo[i].port ==port)
				{
					if(m_pFilterInfo[i].buf[0] ==0) return true;
					mreplace(buf, len, m_pFilterInfo[i].buf, ' ');
				}
			}
		}
	}

	return false;
}

int CSocketFilter::FilterAccept(int sd)
{
	char ip[16];
	int port =GetLocalPortBySocket(sd);
	GetRemoteIPBySocket(sd, ip);

	char app[128];

	GetFileName(app);
	
	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_ACCEPT)
			continue;
		if(m_pFilterInfo[i].app[0] =='*' || !strcmpi(m_pFilterInfo[i].app, app))
		{
			if(m_pFilterInfo[i].ip[0] =='*' || !strcmpi(m_pFilterInfo[i].ip, ip))
			{
				if(m_pFilterInfo[i].port ==0 || m_pFilterInfo[i].port ==port)
					return true;
			}
		}
	}

	return false;
}
