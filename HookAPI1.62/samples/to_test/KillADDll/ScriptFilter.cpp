#include "stdafx.h"
#include <malloc.h>
#include "util.h"
#include "const.h"
#include "Filter.h"

typedef int (WINAPI *FILTERFUNC)();

// int WINAPI ConnectFilter(char *ip, int port);
typedef int (WINAPI *CONNECTFILTERFUNC)(char *, int);
// int WINAPI BindFilter(int port);
typedef int (WINAPI *BINDFILTERFUNC)(int);
// int WINAPI AcceptFilter(int sd, char *ip, int port);
typedef int (WINAPI *ACCEPTFILTERFUNC)(int, char *, int);
// int WINAPI RecvFilter(int sd, char *data, int data_len, char *data1, int *pdata1_len);
typedef int (WINAPI *RECVFILTERFUNC)(int, char *, int, char *, int *);
// int WINAPI SendFilter(int sd, char *data, int data_len, char *data1, int *pdata1_len);
typedef int (WINAPI *SENDFILTERFUNC)(int, char *, int, char *, int *);

typedef struct
{
	int type;
	HANDLE hModule;
	CONNECTFILTERFUNC ConnectFilter;
	BINDFILTERFUNC BindFilter;
	ACCEPTFILTERFUNC AcceptFilter;
	RECVFILTERFUNC RecvFilter;
	SENDFILTERFUNC SendFilter;
}FILTER_INFO;

HANDLE *pFilter
FILTER_INFO *g_pFilterInfo =NULL;
int g_filter_info_count =0;
///////////////// ok//////////////////
int CryptInitDLL()
{
	if(crypt_dll_file[0] ==0) return 0;
	if((hModCrypt =LoadLibrary(crypt_dll_file)) !=NULL)
	{
		myCryptData =(CRYPTDATA)GetProcAddress((HMODULE)hModCrypt, "CryptData");
		myDecryptData =(DECRYPTDATA)GetProcAddress((HMODULE)hModCrypt, "DecryptData");
	}
	else WriteLog("LoadLibrary crypt dll %s failed!", crypt_dll_file);

	return 0;
}

int CryptFreeDLL()
{
	if(hModCrypt)
	{
		FreeLibrary((HINSTANCE)hModCrypt);
		hModCrypt =NULL;
	}

	return 0;
}

int CryptInit()
{
	char init_file[128];
	char temp[100], temp1[128];

	//WriteLog("CryptInit...");
	if(g_szDllPath[0] ==0 || g_crypt_info_count >0)
		return 0;
	wsprintf(init_file, "%s\\crypt.ini", g_szDllPath);
	if(GetPrivateProfileString("SETUP", "dll", "", temp1, sizeof(temp1), init_file) >0)
		wsprintf(crypt_dll_file, "%s\\%s", g_szDllPath, temp1);
	
	g_crypt_info_count =0;

	while(1)
	{
		wsprintf(temp, "APP%d", g_crypt_info_count+1);
		if(GetPrivateProfileString("SETUP", temp, "", temp1, sizeof(temp1), init_file) <1)
			break;
		if(g_pCryptInfo ==NULL) g_pCryptInfo =(CRYPT_INFO *)malloc(sizeof(CRYPT_INFO));
		else g_pCryptInfo =(CRYPT_INFO *)realloc(g_pCryptInfo, sizeof(CRYPT_INFO)*(g_crypt_info_count+1));
		if(g_pCryptInfo ==NULL) return -1;
		memset(&g_pCryptInfo[g_crypt_info_count], 0, sizeof(CRYPT_INFO));
		strupr(temp1);
		strcpy(g_pCryptInfo[g_crypt_info_count].app, temp1);
		wsprintf(temp, "IP%d", g_crypt_info_count+1);
		GetPrivateProfileString("SETUP", temp, "", g_pCryptInfo[g_crypt_info_count].ip, 16, init_file);
		wsprintf(temp, "LOCAL_PORT_TYPE%d", g_crypt_info_count+1);
		g_pCryptInfo[g_crypt_info_count].server_port =GetPrivateProfileInt("SETUP", temp, 0, init_file);
		GetPrivateProfileString("SETUP", temp, "", g_pCryptInfo[g_crypt_info_count].local_port_type, 2, init_file);
		//WriteLog("crypt:%d", g_crypt_info_count);
		g_crypt_info_count++;
	}

	return 0;
}

// 检查是否需要加解密
BOOL CryptCheck(char *remote_ip, int local_port, int remote_port)
{
	int i;
	int port;
	
	//WriteLog("CryptCheck...");
	for(i =0; i<g_crypt_info_count; i++)
	{
//WriteLog("g_szAppBaseName:%s, app:%s, ip:%s, port:%d, local_port_type:%s", g_szAppBaseName, g_pCryptInfo[i].app, g_pCryptInfo[i].ip, g_pCryptInfo[i].server_port,g_pCryptInfo[i].local_port_type);
		if(strcmp(g_pCryptInfo[i].app, g_szAppBaseName)==0 || strcmp(g_pCryptInfo[i].app, "ALL") ==0)
		{
			if(g_pCryptInfo[i].ip[0] ==0 || strcmp(g_pCryptInfo[i].ip, remote_ip) ==0)
			{
				if(g_pCryptInfo[i].server_port ==0) return true;
				if(g_pCryptInfo[i].local_port_type[0] =='S' || g_pCryptInfo[i].local_port_type[0] =='s')
					port =local_port;
				else port =remote_port;
				if(g_pCryptInfo[i].server_port ==port)
					return true;
			}
		}
	}

	return false;
}

// CryptData
// return: 0 - data not changed, 1 - data crypted, <0 - failed
int CryptData(char *data, int data_len, char *data_crypted, int *pdata_crypted_len)
{
	int ret =0;

	CryptInitDLL();
	if(myCryptData)
	{
		ret = myCryptData(data, data_len, data_crypted, pdata_crypted_len);
		WriteLog("Dll crypt, ret=%d", ret);
	}
	else
	{
		for(int i =0; i<data_len; i++)
			data_crypted[i] ^=data[i];
		*pdata_crypted_len =data_len;
		ret =data_len;
		WriteLog("Local Crypt, ret=%d", ret);
	}
	CryptFreeDLL();
	return ret;
}

// DecryptData
// return: 0 - data not changed, 1 - data decrypted, <0 - failed
int DecryptData(char *data, int data_len, char *data_decrypted, int *pdata_decrypted_len)
{
	int ret =0;

	CryptInitDLL();
	if(myDecryptData)
	{
		ret = myDecryptData(data, data_len, data_decrypted, pdata_decrypted_len);
		WriteLog("Dll Decrypt, ret=%d", ret);
	}
	else
	{
		for(int i =0; i<data_len; i++)
			data_decrypted[i] ^=data[i];
		*pdata_decrypted_len =data_len;
		ret =data_len;
		WriteLog("Local Decrypt, ret=%d", ret);
	}
	CryptFreeDLL();

	return ret;
}

int CryptExit()
{
	g_crypt_info_count =0;

	if(g_pCryptInfo)
	{
		free(g_pCryptInfo);
		g_pCryptInfo =NULL;
	}

	return 0;
}
