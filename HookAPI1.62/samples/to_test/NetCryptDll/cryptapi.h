#pragma once

#include "rsaref.h"

#define KEY_TYPE_PAIR		0
#define KEY_TYPE_PUBLIC		1
#define KEY_TYPE_PRIVATE	2

class CRsaCrypt
{
public:
	CRsaCrypt(char *m_key_name, char *password, int key_type);
	~CRsaCrypt();
	
	R_RANDOM_STRUCT m_random_struct;
	R_RSA_PUBLIC_KEY m_public_key;
	R_RSA_PRIVATE_KEY m_private_key;
	char m_key_name[60];
	char m_password[20];
	int m_key_type;

	int InitRandomStruct();
	int GenRandomBytes(UCHAR  *randomBytes, ULONG  randomBytesLen);
	int GenKeyPair(int bits);
	int ReadKey();
	int PublicEncrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len);
	int PublicDecrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len);
	int PrivateEncrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len);
	int PrivateDecrypt(unsigned char *input ,int input_len, unsigned char *output, int *output_len);
};

/*class CRsaCryptManager
{
public:
	int m_ras_crypt_count;
	CRsaCrypt *m_prsa_crypt;
	int *m_psocket;
	CIRITCAL_SECTION m_cs;

	CRsaCryptManager();
	~CRsaCryptManager();
	CRsaCrypt *AddItem(char *key_name, char *password, int key_type, int socket);
	int DeleteItem(int socket);
	int DeleteAllItems();
	CRsaCrypt *GetItem(int sd);
};
*/