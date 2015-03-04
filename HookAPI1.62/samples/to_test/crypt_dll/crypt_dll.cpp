// crypt_dll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "crypt_dll.h"
#include "des.h"
#include "keymanager.h"

unsigned char des_key[8];

#define DEFAULT_KEY	(unsigned char *)"&@$7/;f`"

DES g_des;
CKeyManager g_key;

int WINAPI CryptInit()
{
	return 0;
}

int WINAPI CryptExit()
{
	g_key.RemoveAllKey();

	return 0;
}

// 连接服务器并从服务器端接收到密钥后的处理
int WINAPI CryptOnConnected(int sd, unsigned char *key, int key_len)
{
	g_key.AddKey(sd, key, key_len);
	return 0;
}

// 接受客户端请求后的处理，需要此函数填充密钥
int WINAPI CryptOnAccepted(int sd, unsigned char *out_key, int *pkey_len)
{
	*pkey_len =g_key.GenKey(out_key);
	if(*pkey_len >0)
		g_key.AddKey(sd, out_key, *pkey_len);

	return *pkey_len;
}

// 连接断开后的处理
int WINAPI CryptOnConnectionClosed(int sd, char *out_key, int *pkey_len)
{
	g_key.RemoveKey(sd);

	return 0;
}

// 加密数据
int WINAPI CryptData(int sd, char *data, int len, char *data_crypted, int *pcrypted_len)
{
	KEY_INFO *pkey;
	if(pkey =g_key.GetKey(sd))
	{
		g_des.encrypt(pkey->key, (unsigned char *)data, (unsigned char *)data_crypted, len/8);
		*pcrypted_len =len;
	}
	else *pcrypted_len =0;

	return len;
}

// 解密数据
int WINAPI DecryptData(int sd, char *data, int len, char *data_decrypted, int *pdecrypted_len)
{
	KEY_INFO *pkey;
	if(pkey =g_key.GetKey(sd))
	{
		g_des.encrypt(pkey->key, (unsigned char *)data, (unsigned char *)data_decrypted, len/8);
		*pdecrypted_len =len;
	}
	else *pdecrypted_len =0;

	return len;
}
