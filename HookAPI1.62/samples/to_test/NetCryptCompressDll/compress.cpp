#include "stdafx.h"
#include <malloc.h>
#include "util.h"
#include "const.h"
#include "compress.h"

char compress_dll_file[128];

CompressFilter::CompressFilter()
{
	hModCompress =NULL;
	myCompressData =NULL;
	myDecompressData =NULL;
	m_pCompressInfo =NULL;
	m_compress_info_count =0;

	char init_file[128];
	char temp[100], temp1[128];
	
	if(g_szDllPath[0] ==0 || m_compress_info_count >0)
		return;
	//WriteLog("CompressInit...");
	wsprintf(init_file, "%s\\compress.ini", g_szDllPath);
	if(GetPrivateProfileString("SETUP", "dll", "", temp1, sizeof(temp1), init_file) >0)
		wsprintf(compress_dll_file, "%s\\%s", g_szDllPath, temp1);

	m_compress_info_count =0;

	while(1)
	{
		wsprintf(temp, "APP%d", m_compress_info_count+1);
		if(GetPrivateProfileString("SETUP", temp, "", temp1, sizeof(temp1), init_file) <1)
			break;
		if(m_pCompressInfo ==NULL) m_pCompressInfo =(COMPRESS_INFO *)malloc(sizeof(COMPRESS_INFO));
		else m_pCompressInfo =(COMPRESS_INFO *)realloc(m_pCompressInfo, sizeof(COMPRESS_INFO)*(m_compress_info_count+1));
		if(m_pCompressInfo ==NULL) return;
		memset(&m_pCompressInfo[m_compress_info_count], 0, sizeof(COMPRESS_INFO));
		strupr(temp1);
		strcpy(m_pCompressInfo[m_compress_info_count].app, temp1);
		wsprintf(temp, "IP%d", m_compress_info_count+1);
		GetPrivateProfileString("SETUP", temp, "", m_pCompressInfo[m_compress_info_count].ip, 16, init_file);
		wsprintf(temp, "PORT%d", m_compress_info_count+1);
		m_pCompressInfo[m_compress_info_count].server_port =GetPrivateProfileInt("SETUP", temp, 0, init_file);
		m_compress_info_count++;
		//WriteLog("compress:%d", m_compress_info_count);
	}

	if(compress_dll_file[0] ==0) return;
	if((hModCompress =LoadLibrary(compress_dll_file)) !=NULL)
	{		
		myCompressData =(COMPRESSDATA)GetProcAddress((HMODULE)hModCompress, "CompressData");
		myDecompressData =(DECOMPRESSDATA)GetProcAddress((HMODULE)hModCompress, "DecompressData");
	}
	else WriteLog("LoadLibrary compress dll %s failed!", compress_dll_file);
}

CompressFilter::~CompressFilter()
{
	if(hModCompress)
	{
		FreeLibrary((HINSTANCE)hModCompress);
		hModCompress =NULL;
	}

	if(m_pCompressInfo)
	{
		free(m_pCompressInfo);
		m_pCompressInfo =NULL;
	}
	m_compress_info_count =0;
}


// 检查是否需要压缩解压
BOOL CompressFilter::CompressCheck(char *remote_ip, int local_port, int remote_port)
{
	int i;
	int port;

	//WriteLog("Compress check...");

	for(i =0; i<m_compress_info_count; i++)
	{
		if(strcmp(m_pCompressInfo[i].app, g_szAppBaseName)==0 || strcmp(m_pCompressInfo[i].app, "ALL") ==0)
		{
			if(m_pCompressInfo[i].ip[0] ==0 || strcmp(m_pCompressInfo[i].ip, remote_ip) ==0)
			{
				if(m_pCompressInfo[i].server_port ==0) return true;
				if(m_pCompressInfo[i].local_port_type[0] =='S' || m_pCompressInfo[i].local_port_type[0] =='s')
					port =local_port;
				else port =remote_port;
				if(m_pCompressInfo[i].server_port ==port)
					return true;
			}
		}
	}
	return false;
}

// CompressData
// return: 0 - data not changed, 1 - data compressed, <0 - failed
int CompressFilter::CompressData(char *data, int data_len, char *data_compressed, int *pdata_compressed_len)
{
	int ret =0;
	if(myCompressData)
		ret = myCompressData(data, data_len, data_compressed, pdata_compressed_len);

	return ret;
}

// DecompressData
// return: 0 - data not changed, 1 - data decompressed, <0 - failed
int CompressFilter::DecompressData(char *data, int data_len, char *data_decompressed, int *pdata_decompressed_len)
{
	int ret =0;

	if(myDecompressData)
		ret = myDecompressData(data, data_len, data_decompressed, pdata_decompressed_len);

	return ret;
}
