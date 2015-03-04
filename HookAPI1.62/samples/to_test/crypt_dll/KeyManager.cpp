#include "stdafx.h"
#include <time.h>

#include "keymanager.h"


CKeyManager::CKeyManager(void)
{
	m_pkey_info =NULL;
	m_key_info_count =0;
	InitializeCriticalSection(&m_cs_key);
}

CKeyManager::~CKeyManager(void)
{
	RemoveAllKey();
	DeleteCriticalSection(&m_cs_key);
}

// 添加一个请求连接数据
int CKeyManager::AddKey(int sd, unsigned char *key, int key_len)
{
	KEY_INFO **ppkey =&m_pkey_info;
	EnterCriticalSection(&m_cs_key);
	while(1)
	{
		if(*ppkey ==NULL)
		{
			*ppkey =(KEY_INFO *)malloc(sizeof(KEY_INFO));
			memset(*ppkey, 0, sizeof(KEY_INFO));
			(*ppkey)->sd =sd;
			memcpy((*ppkey)->key, key, key_len);
			(*ppkey)->key_len =key_len;
			break;
		}
		ppkey =&((*ppkey)->pnext);
	}
	LeaveCriticalSection(&m_cs_key);

	return 0;
}

// 删除一个请求连接数据
int CKeyManager::RemoveKey(int sd)
{
	KEY_INFO *pkey =m_pkey_info, *pprev_info =NULL, **ppkey =&m_pkey_info;
	EnterCriticalSection(&m_cs_key);
	while(pkey)
	{
		if(pkey->sd ==sd)
		{
			if(pprev_info)
				pprev_info->pnext =pkey->pnext;
			free(pkey);
			*ppkey =NULL;
			break;
		}
		pprev_info =pkey;
		pkey =pkey->pnext;
		ppkey =&pkey->pnext;
	}

	LeaveCriticalSection(&m_cs_key);

	return 0;
}

void CKeyManager::RemoveAllKey()
{
	KEY_INFO *pkey =m_pkey_info, *pprev_key =NULL;

	EnterCriticalSection(&m_cs_key);
	while(pkey)
	{
		pprev_key =pkey;
		pkey =pkey->pnext;
		free(pprev_key);
	}
	m_pkey_info =NULL;

	LeaveCriticalSection(&m_cs_key);

}


int CKeyManager::GenKey(unsigned char * key)
{
	srand((unsigned int)time(NULL));
	for(int i =0; i<16; i++)   // 3des key size =16
		key[i] =rand()%255+1;
	
	return i;
}

KEY_INFO *CKeyManager::GetKey(int sd)
{
	KEY_INFO *pkey =m_pkey_info;

	EnterCriticalSection(&m_cs_key);
	
	while(pkey)
	{
		if(pkey->sd ==sd)
			return pkey;
		pkey =pkey->pnext;
	}

	LeaveCriticalSection(&m_cs_key);

	return pkey;
}
