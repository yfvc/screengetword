#ifndef __UTIL_H__
#define __UTIL_H__

#include <winsock.h>

typedef struct
{
	char app_name[128];
	char ip[16];
	char password[9];
}PASSWORD_DATA;

int ipcmp(char *szip1, char *szip2);
ULONG GetIntIP(char *szip);
ULONG my_atoi(char *p);

void GetFileName(char *fname);
void WriteLog(char *fmt,...);
int CheckPassword(char *, char *, char *password);
int FindData(unsigned char *, int , unsigned char *, int, int);
char *upper_case(char *p, int len);
int mreplace(char *buf, int len, char *str, char c);

int EncryptData(int algrithm, char *password, unsigned char *inbuf, int inbuf_len, unsigned char *outbuf, int *outbuf_len);
int DecryptData(int algrithm, char *password, unsigned char *inbuf, int inbuf_len, unsigned char *outbuf, int *outbuf_len);

int GetLocalPortBySocket(SOCKET s);
int GetLocalIPBySocket(SOCKET s, char *ip);
int GetRemotePortBySocket(SOCKET s);
int GetRemoteIPBySocket(SOCKET s, char *ip);

#endif
