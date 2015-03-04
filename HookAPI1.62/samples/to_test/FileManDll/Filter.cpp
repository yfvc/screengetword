#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "Filter.h"

char g_dll_path[128];

CFileFilter::CFileFilter()
{
	m_pFilterInfo =NULL;
	m_nFilterInfoCount =0;
	
	FILE *fp;

	if(GetProfileString("HookAPI", "dll_path", "", g_dll_path, sizeof(g_dll_path)) ==0)
		return;
	char init_file[128];

	wsprintf(init_file, "%s\\filterfile.ini", g_dll_path);
	//WriteLog("FilterInit:init_file=%s", init_file);
	if((fp =fopen(init_file, "r")) ==NULL)
	{
		WriteLog("open init file %s failed!", init_file);
		return;
	}
	char buf[300], *p, app[40], type[10], password[21];
	while(!feof(fp) && fgets(buf, sizeof(buf), fp))
	{
		if(strlen(buf) <5 || buf[0] =='#') continue;
		if(buf[strlen(buf)-1] =='\r' || buf[strlen(buf)-1] =='\n')
			buf[strlen(buf)-1] =0;
		if(buf[strlen(buf)-2] =='\r' || buf[strlen(buf)-2] =='\n')
			buf[strlen(buf)-2] =0;
		
		p =strtok(buf, ",");
		if(!p) continue;
		if(strlen(p) >= sizeof(type)) continue;
		strcpy(type, p);
		//WriteLog("type=%s", type);
		
		p =strtok(NULL, ",");
		if(!p) continue;
		if(strlen(p) >=sizeof(app)) continue;
		strcpy(app, p);
		strupr(app);
		//WriteLog("app=%s", app);


		//p =strtok(NULL, ",");
		//if(!p) continue;
		int f_permit =0;//*p-'0';

		password[0] =0;
		p =strtok(NULL, ",");
		if(p)
		{
			if(strlen(p) >=sizeof(password)) continue;
			strcpy(password, p);
		}

		if(m_pFilterInfo)
			m_pFilterInfo =(FILTER_INFO *)realloc(m_pFilterInfo, (m_nFilterInfoCount+1)*sizeof(FILTER_INFO));
		else
			m_pFilterInfo =(FILTER_INFO *)malloc((m_nFilterInfoCount+1)*sizeof(FILTER_INFO));
		if(!strcmpi(type, "execute"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_EXECUTE;
		else if(!strcmpi(type, "open"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_OPEN;
		else if(!strcmpi(type, "create"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_CREATE;
		else if(!strcmpi(type, "write"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_WRITE;
		else if(!strcmpi(type, "read"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_READ;
		else if(!strcmpi(type, "copy"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_COPY;
		else if(!strcmpi(type, "move"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_MOVE;
		else if(!strcmpi(type, "chdir"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_CHDIR;
		else if(!strcmpi(type, "opendir"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_OPENDIR;
		else if(!strcmpi(type, "createdir"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_CREATEDIR;
		else if(!strcmpi(type, "movedir"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_MOVEDIR;
		else if(!strcmpi(type, "hideprocess"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_HIDEPROCESS;
		else if(!strcmpi(type, "execrypt"))
			m_pFilterInfo[m_nFilterInfoCount].type =FILTER_EXECRYPT;
		else
			m_pFilterInfo[m_nFilterInfoCount].type =-1;

		strcpy(m_pFilterInfo[m_nFilterInfoCount].app, app);
		m_pFilterInfo[m_nFilterInfoCount].f_permit =f_permit;
		strcpy(m_pFilterInfo[m_nFilterInfoCount].password, password);

		m_nFilterInfoCount++;
	}
	//WriteLog("filercount=%d", m_nFilterInfoCount);
}

CFileFilter::~CFileFilter()
{
	if(m_pFilterInfo)
		free(m_pFilterInfo);
}

int CFileFilter::FilterName(char *name, int type)
{
	char name2[512];

	strcpy(name2, name);
	strupr(name2);

	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		//WriteLog("type=%d, name2=%s, app=%s", type, name2, m_pFilterInfo[i].app);
		if(m_pFilterInfo[i].type !=type)
			continue;
		if(strstr(name2, m_pFilterInfo[i].app))
		{
			WriteLog("FilterName ok! %s", name);
			return true;
		}
	}
	return false;
}

int CFileFilter::FilterExecute(char *cmd)
{
	return FilterName(cmd, FILTER_EXECUTE);
}

int CFileFilter::FilterExeCrypt(char *cmd, char *new_cmd)
{
	char cmd2[512], password[9], file[128], file2[128], app[128], app2[128];

	strcpy(cmd2, cmd);
	strupr(cmd2);
	split_cmd(cmd2, app, file);

	for(int i=0; i<m_nFilterInfoCount; i++)
	{
		if(m_pFilterInfo[i].type !=FILTER_EXECRYPT)
			continue;
		if(strstr(file, m_pFilterInfo[i].app))
		{
			if(GetPassword(m_pFilterInfo[i].app, password) <0)
				return -1;
			if(MyDecryptFile(file, file2, password) <0)
				return -2;
			wsprintf(new_cmd, "\"%s\" \"%s\"", app, file2);

			return 1;
		}
		else if(strstr(app, m_pFilterInfo[i].app))
		{
			if(GetPassword(m_pFilterInfo[i].app, password) <0)
				return -1;
			if(MyDecryptFile(app, app2, password) <0)
				return -2;
			wsprintf(new_cmd, "\"%s\" \"%s\"", app2, file);
			return 1;
		}
	}
	return 0;
}

int CFileFilter::FilterOpenFile(char *fname)
{
	return FilterName(fname, FILTER_OPEN);
}

int CFileFilter::FilterCreateFile(char *fname)
{
	if(FilterName(fname, FILTER_OPEN)) return TRUE;

	return FilterName(fname, FILTER_CREATE);
}

int CFileFilter::FilterWriteFile(char *fname)
{
	return FilterName(fname, FILTER_WRITE);
}

int CFileFilter::FilterReadFile(char *fname)
{
	return FilterName(fname, FILTER_READ);
}

int CFileFilter::FilterCopyFile(char *fsrc, char *fdst)
{
	return FilterName(fsrc, FILTER_COPY);
}

int CFileFilter::FilterMoveFile(char *fname, char *fname2)
{
	return FilterName(fname, FILTER_MOVE);
}

int CFileFilter::FilterChDir(char *dir)
{
	return FilterName(dir, FILTER_CHDIR);
}

int CFileFilter::FilterOpenDir(char *dir)
{
	return FilterName(dir, FILTER_OPENDIR);
}

int CFileFilter::FilterCreateDir(char *dir)
{
	return FilterName(dir, FILTER_CREATEDIR);
}

int CFileFilter::FilterDeleteFile(char *fname)
{
	return FilterName(fname, FILTER_DELETE);
}
