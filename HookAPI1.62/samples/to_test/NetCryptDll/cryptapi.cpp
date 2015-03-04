#include "stdafx.h"
#include <stdio.h>
#include <time.h>

#include "rsaref.h"
#include "cryptapi.h"

CRsaCrypt::CRsaCrypt(char *key_name, char *password, int key_type)
{
	strcpy(m_key_name, key_name);
	strcpy(m_password, password);
	m_key_type =key_type;
}

CRsaCrypt::~CRsaCrypt()
{
}

int CRsaCrypt::ReadKey()
{
	char key_name[128];
	FILE *fp;

	wsprintf(key_name, "%s.%d", m_key_name, m_key_type);

	if((fp =fopen(key_name, "rb")) ==NULL)
		return -1;
	if(m_key_type ==KEY_TYPE_PAIR || m_key_type ==KEY_TYPE_PUBLIC)
	{
		if(fread(&m_public_key , sizeof(m_public_key), 1, fp) !=1)
		{
			fclose(fp);
			return -2;
		}
		if(m_key_type ==KEY_TYPE_PAIR)
		if(fread(&m_private_key , sizeof(m_private_key), 1, fp) !=1)
		{
			fclose(fp);
			return -2;
		}
	}
	else if(m_key_type ==KEY_TYPE_PRIVATE)
	{
		if(fread(&m_private_key , sizeof(m_private_key), 1, fp) !=1)
		{
			fclose(fp);
			return -2;
		}
	}
	else
	{
		fclose(fp);
		return -10;
	}

	fclose(fp);

	return 0;
}

int CRsaCrypt::InitRandomStruct ()
{
	unsigned int bytesNeeded;
	long t_time;

	R_RandomInit (&m_random_struct);
	/*
	 *Initialize with time seed.
	 */
	while (1) {
		R_GetRandomBytesNeeded (&bytesNeeded, &m_random_struct);
		if (bytesNeeded == 0)
			break;

		time(&t_time);
		R_RandomUpdate (&m_random_struct,(unsigned char*) &t_time, sizeof(t_time));
	}

	return 0;
}

int  CRsaCrypt::GenRandomBytes(UCHAR  *randomBytes, ULONG  randomBytesLen)
{
	InitRandomStruct ();
	if(randomBytesLen <1 || randomBytesLen>256)
		return -1;
	return  R_GenerateBytes(randomBytes,randomBytesLen,&m_random_struct);
}

int CRsaCrypt::GenKeyPair(int bits)
{
	R_RSA_PROTO_KEY protoKey;

	if(bits <=0 || bits%128 !=0)
		bits =1024;
	protoKey.bits = bits;
	protoKey.useFermat4 = 1;

	InitRandomStruct();

	return R_GeneratePEMKeys(&m_public_key, &m_private_key, &protoKey, &m_random_struct);
}

int CRsaCrypt::PublicEncrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len)
{
	InitRandomStruct();
	return RSAPublicEncrypt(output, (unsigned int *)output_len, input, (unsigned int)input_len, &m_public_key, &m_random_struct);
}

int CRsaCrypt::PublicDecrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len)
{
	return RSAPublicDecrypt (output, (unsigned int *)output_len, input, (unsigned int)input_len, &m_public_key);
}

int CRsaCrypt::PrivateEncrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len)
{
	return RSAPrivateEncrypt (output, (unsigned int *)output_len, input, (unsigned int)input_len, &m_private_key);
}

int CRsaCrypt::PrivateDecrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len)
{
	return RSAPrivateDecrypt (output, (unsigned int *)output_len, input, (unsigned int)input_len, &m_private_key);
}

/*CRsaCryptManager::CRsaCryptManager()
{
	m_rsa_crypt_count =0;
	m_prsa_crypt =NULL;

	InitializeCriticalSection(&m_cs);
}

CRsaCryptManager::~CRsaCryptManager()
{
	DeleteAllItems();
	DeleteCriticalSection(&m_cs_key);
}

*/