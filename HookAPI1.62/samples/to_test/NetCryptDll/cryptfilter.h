#ifndef _Crypt_H_

typedef int (WINAPI *CRYPTDATA)(char *, int, char *, int *);
typedef int (WINAPI *DECRYPTDATA)(char *, int, char *, int *);

typedef struct
{
	char app[128];
	char ip[16];
	char local_port_type[2];
	int server_port;
}CRYPT_INFO;

class CryptFilter
{
public:
	CryptFilter();
	~CryptFilter();
	BOOL CryptCheck(char *remote_ip, int local_port, int remote_port);
	int CryptData(char *data, int data_len, char *data_crypted, int *pdata_crypted_len);
	int DecryptData(char *data, int data_len, char *data_decrypted, int *pdata_decrypted_len);

	CRYPTDATA myCryptData;
	DECRYPTDATA myDecryptData;
	HANDLE hModCrypt;
	CRYPT_INFO *m_pCryptInfo;
	int m_crypt_info_count;
};

#endif
