#include "stdafx.h"
#include <stdio.h>
#include <io.h>
#include "resource.h"

#include "util.h"
extern HINSTANCE g_hInstance;

void GetFileName(char *fname)
{
	char temp[200];

	GetModuleFileName(NULL, temp, sizeof(temp));
	int i =strlen(temp);
	while(i >0 && temp[i-1] !='\\' && temp[i-1] !=':') i--;

	strcpy(fname, &temp[i]);
}

// 如果是win9x，不能使用fopen函数
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
	
	SetFilePointer((HANDLE)hFile, 0, NULL, FILE_END);

	wsprintf(temp, "mydll.dll:%s:", modname);
	DWORD dw;
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	
	va_start(args,fmt);
	vsprintf(temp, fmt, args);
	va_end(args);

	WriteFile(hFile, temp, strlen(temp), &dw, NULL);

	wsprintf(temp, "\r\n");
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);

	CloseHandle(hFile);
}

void WriteLog9x(char *msg)
{
	char modname[200];

	char temp[5000];
	HANDLE hFile;

	GetModuleFileName(NULL, modname, sizeof(modname));

	if((hFile =CreateFile("c:\\hookapi.log", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0)
	{
		return;
	}
	
	SetFilePointer((HANDLE)hFile, 0, NULL, FILE_END);

	wsprintf(temp, "mydll.dll:%s:", modname);
	DWORD dw;
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	
	WriteFile(hFile, msg, strlen(msg), &dw, NULL);

	wsprintf(temp, "\r\n");
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);

	CloseHandle(hFile);
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
		SetDlgItemText(hDlg, IDC_IP, pdata->ip);
		SetWindowPos(hDlg, NULL, 300, 300, 0, 0, SWP_NOSIZE);
		SendDlgItemMessage(hDlg, IDE_PASSWORD, EM_SETLIMITTEXT, 8, 0L);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDE_PASSWORD, password, sizeof(password));
			if(strcmp(password, pdata->password) !=0)
			{
				MessageBox(hDlg, "密码不对", pdata->app_name, MB_OK);
				return TRUE;
			}
			EndDialog(hDlg, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
		}
		break;
	}
	return FALSE;
}

int CheckPassword(char *app, char *ip, char *password)
{
	PASSWORD_DATA data;
	strcpy(data.app_name, app);
	strcpy(data.ip, ip);
	strcpy(data.password, password);
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

