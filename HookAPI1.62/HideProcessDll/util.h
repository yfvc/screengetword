#ifndef __UTIL_H__
#define __UTIL_H__

#include <winsock.h>

typedef struct
{
	char app_name[128];
	char *p_password;
}PASSWORD_DATA;

int ipcmp(char *szip1, char *szip2);
ULONG GetIntIP(char *szip);
ULONG my_atoi(char *p);

void GetFileName(char *fname);
void WriteLog(char *fmt,...);
int GetPassword(char *, char *);
int FindData(unsigned char *, int , unsigned char *, int, int);
char *upper_case(char *p, int len);
int mreplace(char *buf, int len, char *str, char c);

int EncryptData(int algrithm, char *password, unsigned char *inbuf, int inbuf_len, unsigned char *outbuf, int *outbuf_len);
int DecryptData(int algrithm, char *password, unsigned char *inbuf, int inbuf_len, unsigned char *outbuf, int *outbuf_len);

int GetLocalPortBySocket(SOCKET s);
int GetLocalIPBySocket(SOCKET s, char *ip);
int GetRemotePortBySocket(SOCKET s);
int GetRemoteIPBySocket(SOCKET s, char *ip);
int GetProcessBaseName(HANDLE hProcess, char *name);
char * GetErrString(char *str, DWORD errcode);
int split_cmd(char *cmd, char *app, char *file);
int MyDecryptFile(char *src_file, char *dst_file, char *password);
int GetFileNameFromhWnd(HWND hWnd, char *fname);

#endif
