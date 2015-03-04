#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

#include "resource.h"
#include "util.h"
#include "psapi.h"

extern HINSTANCE g_hInstance;

void GetFileName(char *fname)
{
	char temp[200];

	GetModuleFileName(NULL, temp, sizeof(temp));
	int i =strlen(temp);
	while(i >0 && temp[i-1] !='\\' && temp[i-1] !=':') i--;
	strcpy(fname, &temp[i]);
	strupr(fname);
}

void WriteLog(char *fmt,...)
{
	FILE *fp;
	va_list args;
	char modname[200];

	if((fp =fopen("c:\\hookapi.log", "a")) !=NULL)
	{		
		va_start(args,fmt);

		GetModuleFileName(NULL, modname, sizeof(modname));
		if(!strstr(modname, "HookAPI"))
		{
			fprintf(fp, "mydll.dll:%s:", modname);
			vfprintf(fp, fmt, args);
			fprintf(fp, "\n");
		}
		fclose(fp);
		
		va_end(args);
	}
}

int ipcmp(char *szip1, char *szip2)
{
	ULONG ip1 =GetIntIP(szip1);
	ULONG ip2 =GetIntIP(szip2);
	if(ip1 > ip2) return 1;
	else if(ip1 <ip2) return -1;
	else return 0;
}

ULONG GetIntIP(char *szip)
{
	char *p, *p1;
	char ip[16];
	int i;
	ULONG ii[4];
	i =0;
	strcpy(ip, szip);
	p =ip;
	while(*p && i<4)
	{
		p1 =p;
		while(*p && *p !='.') p++;
		*p =0;
		ii[i] =my_atoi(p1);
		p ++;
		i++;
	}
	
	return ii[0]*256*256*256+ii[1]*256*256+ii[2]*256+ii[3];
}

ULONG my_atoi(char *p)
{
	ULONG i;
	
	i =0;
	while(*p)
	{
		i =(i+*p-'0')*10;
		p++;
	}
	i =i/10;
	
	return i;
}

BOOL CALLBACK PasswordDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static PASSWORD_DATA *pdata;
	char password[9];

	switch(msg)
	{
	case WM_INITDIALOG:
		pdata =(PASSWORD_DATA *)lParam;
		SetDlgItemText(hDlg, IDC_APP_NAME, pdata->app_name);
		SetWindowPos(hDlg, NULL, 300, 300, 0, 0, SWP_NOSIZE);
		SendDlgItemMessage(hDlg, IDE_PASSWORD, EM_SETLIMITTEXT, 8, 0L);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			if(GetDlgItemText(hDlg, IDE_PASSWORD, password, sizeof(password)) ==0)
				return TRUE;
			strcpy(pdata->p_password, password);
			EndDialog(hDlg, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
		}
		break;
	}
	return FALSE;
}

int GetPassword(char *app, char *password)
{
	PASSWORD_DATA data;
	strcpy(data.app_name, app);
	data.p_password =password;
	if(DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_PASSWORD), NULL, PasswordDlgProc, (LONG)&data) ==IDCANCEL)
		return -1;

	return 0;
}

int EncURL(unsigned char *url, int len, char *new_url)
{
	int k =0;

	for(int i =0; i<len; i++)
	{
		if(url[i] >127)
		{
			wsprintf(&new_url[k], "%%%2X", url[i]);
			k +=3;
		}
		else new_url[k++] =url[i];
	}
	new_url[k] =0;

	return k;
}

int FindData(unsigned char *data1, int len1, unsigned char *data2, int len2, int enc_url)
{
	int i, len_url;
	char url[1024];
	//unsigned char *data =new BYTE[len2];
	int count =0;
	int pos[256], len[256], pos3[256], len3[256];

	int k =0;
	pos [0] =0;
	len [0] =0;
	char *p =(char *)data2;

	for(i =0; i<len2; i++)
	{
		if(p[i] !=',') len[k] ++;
		else
		{
			k++;
			pos[k] =i+1;
			len[k] =0;
		}
	}
	count =k+1;

	if(enc_url)
	{
		len_url =EncURL(data2, len2, url);
		k =0;
		pos3[0] =0;
		len3[0] =0;
		char *p =url;
		for(i =0; i<len_url; i++)
		{
			if(p[i] !=',') len3[k] ++;
			else
			{
				k++;
				pos3[k] =i+1;
				len3[k] =0;
			}
		}
	}
	else len_url =len2;
	//WriteLog("url =%s,len_url=%d\ndata1=%s\n####len1=%d", url,len_url, data1,len1);
	i =0;
	while(i+len_url<=len1)
	{
		for(k =0; k<count; k++)
		{
			if(len[k] && memcmp(data1+i, data2+pos[k], len[k]) ==0) return i;
		}
		if(enc_url)
		{
			for(k =0; k<count; k++)
			{
				if(len3[k] && memcmp(data1+i, url+pos3[k], len3[k]) ==0) return i;
			}
		}
		i++;
	}
	//WriteLog("not found!!!");
	//delete data;

	return -1;
}

//加密例子：使用异或，输出长度不变。
int EncryptData(int algrithm, char *password, unsigned char *inbuf, int inbuf_len, unsigned char *outbuf, int *outbuf_len)
{
	int i;

	for(i =0; i<inbuf_len-1; i++)
	{
		outbuf[i] =inbuf[i]^password[i%8];
	}

	*outbuf_len =inbuf_len;

	return *outbuf_len;
}

int DecryptData(int algrithm, char *password, unsigned char *inbuf, int inbuf_len, unsigned char *outbuf, int *outbuf_len)
{
	int i;

	for(i =0; i<inbuf_len; i++)
	{
		outbuf[i] =inbuf[i]^password[i%8];
	}

	*outbuf_len =inbuf_len;

	return *outbuf_len;
}

char *upper_case(char *p, int len)
{
	for(int i =0; i<len; i++)
	{
		if(p[i] <='a' && p[i] >='z') p[i] =p[i]+'A'-'a';
	}

	return p;
}

int mreplace(char *buf, int len, char *str, char c)
{
	if(len <=0) return NULL;
	int f_found =false;

	for(int i =0; i<len-(int)strlen(str); i++)
	{
		if(strnicmp(&buf[i], str, strlen(str))==0)
		{
			if(c) memset(&buf[i], c, strlen(str));
			f_found =true;
		}
	}

	return f_found;
}

char * GetErrString(char *str, DWORD errcode)
{
	LPVOID lpbuf;

	if(FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpbuf,
		0,
		NULL
	))
	{
		lstrcpy(str, (char *)lpbuf);
		LocalFree(lpbuf);
	}

	return str;
}

int GetProcessBaseName(HANDLE hProcess, char *name)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;

	//ObtainSeDebugPrivilege();
	*name =0;
	if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		GetModuleBaseNameA( hProcess, hMods[0], name,128);
	}
	else
	{
		char err[256];
		WriteLog("EnumProcessModules failed! err=%s", GetErrString(err, GetLastError()));
	}

	return 0;
}

int split_cmd(char *cmd, char *app, char *file)
{
	char cmd2[512];

	*app =0;
	*file =0;

	strcpy(cmd2, cmd);
	char *p1, *p=cmd2;

	if(*p =='\"')
	{
		p++;
		p1 =strtok(p, "\"");
		strcpy(app, p);
		if(p1 ==NULL) return 0;
		if(*(p1+1) !=' ') return 1;
		p =p1+2;
	}
	else
	{
		p1 =strtok(p, " ");
		strcpy(app, p);
		if(p1 ==NULL || *p1 ==NULL) return 1;
		p =p1+1;
	}



	return 0;
}

int MyDecryptFile(char *src_file, char *dst_file, char *password)
{
	return 0;
}

int GetFileNameFromhWnd(HWND hWnd, char *fname)
{
	DWORD dwID;

	fname[0] =0;
	if(GetWindowThreadProcessId(hWnd, &dwID) ==0) return -1;
	HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, 0, dwID);
	if(hProcess ==NULL) return -1;
	GetProcessBaseName(hProcess, fname);
	CloseHandle(hProcess);
	
	return 0;
}
