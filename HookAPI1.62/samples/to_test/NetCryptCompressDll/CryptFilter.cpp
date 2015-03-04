#include "stdafx.h"
#include <malloc.h>
#include "util.h"
#include "const.h"
#include "cryptFilter.h"

char crypt_dll_file[128];

CryptFilter::~CryptFilter()
{
	if(hModCrypt)
	{
		FreeLibrary((HINSTANCE)hModCrypt);
		hModCrypt =NULL;
	}

	m_crypt_info_count =0;

	if(m_pCryptInfo)
	{
		free(m_pCryptInfo);
		m_pCryptInfo =NULL;
	}

}

CryptFilter::CryptFilter()
{
	char init_file[128];
	char temp[100], temp1[128];

	hModCrypt =NULL;
	myCryptData =NULL;
	myDecryptData =NULL;
	m_pCryptInfo =NULL;
	m_crypt_info_count =NULL;

	if(g_szDllPath[0] ==0)
		return;
	wsprintf(init_file, "%s\\crypt.ini", g_szDllPath);
	if(GetPrivateProfileString("SETUP", "dll", "", temp1, sizeof(temp1), init_file) >0)
		wsprintf(crypt_dll_file, "%s\\%s", g_szDllPath, temp1);
	
	m_crypt_info_count =0;

	while(1)
	{
		wsprintf(temp, "APP%d", m_crypt_info_count+1);
		if(GetPrivateProfileString("SETUP", temp, "", temp1, sizeof(temp1), init_file) <1)
			break;
		if(m_pCryptInfo ==NULL) m_pCryptInfo =(CRYPT_INFO *)malloc(sizeof(CRYPT_INFO));
		else m_pCryptInfo =(CRYPT_INFO *)realloc(m_pCryptInfo, sizeof(CRYPT_INFO)*(m_crypt_info_count+1));
		if(m_pCryptInfo ==NULL) return;
		memset(&m_pCryptInfo[m_crypt_info_count], 0, sizeof(CRYPT_INFO));
		strupr(temp1);
		strcpy(m_pCryptInfo[m_crypt_info_count].app, temp1);
		wsprintf(temp, "IP%d", m_crypt_info_count+1);
		GetPrivateProfileString("SETUP", temp, "", m_pCryptInfo[m_crypt_info_count].ip, 16, init_file);
		wsprintf(temp, "LOCAL_PORT_TYPE%d", m_crypt_info_count+1);
		m_pCryptInfo[m_crypt_info_count].server_port =GetPrivateProfileInt("SETUP", temp, 0, init_file);
		GetPrivateProfileString("SETUP", temp, "", m_pCryptInfo[m_crypt_info_count].local_port_type, 2, init_file);
		//WriteLog("crypt:%d", m_crypt_info_count);
		m_crypt_info_count++;
	}

	if(crypt_dll_file[0] ==0) return;
	if((hModCrypt =LoadLibrary(crypt_dll_file)) !=NULL)
	{
		myCryptData =(CRYPTDATA)GetProcAddress((HMODULE)hModCrypt, "CryptData");
		myDecryptData =(DECRYPTDATA)GetProcAddress((HMODULE)hModCrypt, "DecryptData");
	}
	else WriteLog("LoadLibrary crypt dll %s failed!", crypt_dll_file);
}

// 检查是否需要加解密
BOOL CryptFilter::CryptCheck(char *remote_ip, int local_port, int remote_port)
{
	int i;
	int port;
	
	//WriteLog("CryptCheck...");
	for(i =0; i<m_crypt_info_count; i++)
	{
//WriteLog("g_szAppBaseName:%s, app:%s, ip:%s, port:%d, local_port_type:%s", g_szAppBaseName, m_pCryptInfo[i].app, m_pCryptInfo[i].ip, m_pCryptInfo[i].server_port,m_pCryptInfo[i].local_port_type);
		if(strcmp(m_pCryptInfo[i].app, g_szAppBaseName)==0 || strcmp(m_pCryptInfo[i].app, "ALL") ==0)
		{
			if(m_pCryptInfo[i].ip[0] ==0 || strcmp(m_pCryptInfo[i].ip, remote_ip) ==0)
			{
				if(m_pCryptInfo[i].server_port ==0) return true;
				if(m_pCryptInfo[i].local_port_type[0] =='S' || m_pCryptInfo[i].local_port_type[0] =='s')
					port =local_port;
				else port =remote_port;
				if(m_pCryptInfo[i].server_port ==port)
					return true;
			}
		}
	}

	return false;
}

// CryptData
// return: 0 - data not changed, 1 - data crypted, <0 - failed
int CryptFilter::CryptData(char *data, int data_len, char *data_crypted, int *pdata_crypted_len)
{
	int ret =0;

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
	return ret;
}

// DecryptData
// return: 0 - data not changed, 1 - data decrypted, <0 - failed
int CryptFilter::DecryptData(char *data, int data_len, char *data_decrypted, int *pdata_decrypted_len)
{
	int ret =0;

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

	return ret;
}
