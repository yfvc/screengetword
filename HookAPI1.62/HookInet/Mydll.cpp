
#include "stdafx.h"
#include <winsock.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <lmcons.h>
//#include <lmalert.h>
#include "mydll.h"
//#include <nb30.h>
//#include <ras.h>

#include "Util.h"


#define MAX_CONNECTIONS 128
#define MAX_PAGE_SIZE 500000L


#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

HINSTANCE g_hInstance;

char *tbuf[MAX_CONNECTIONS];
HINTERNET hbuf[MAX_CONNECTIONS];
DWORD tbuf_sz[MAX_CONNECTIONS];


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{

	return TRUE;
}

int getBufHandle ( HINTERNET h )
	{
	int last_free = -1;
	for ( int n =0;n<MAX_CONNECTIONS; n++)
		{
		if ( hbuf[n] == h )
			return n;
		if ( (hbuf[n] == 0) && (last_free == -1) )
			last_free = n;
		}
	return last_free;
	}

HINTERNET WINAPI cInternetConnectA (
    HINTERNET hInternet, LPCSTR lpszServerName,
    INTERNET_PORT nServerPort, IN LPCSTR lpszUserName OPTIONAL,
    LPCSTR lpszPassword OPTIONAL, DWORD dwService,
    DWORD dwFlags, DWORD dwContext
    )
{
	HINTERNET hResult = InternetConnectA ( hInternet, lpszServerName,
		nServerPort, lpszUserName, lpszPassword, dwService, dwFlags,
		dwContext);
	WriteLog ( "InternetConnectA: servername=%s, port=%d", lpszServerName, nServerPort );
	return hResult;
}

BOOL WINAPI cHttpSendRequestA (
    HINTERNET hRequest, LPCSTR lpszHeaders,
    DWORD dwHeadersLength,LPVOID lpOptional ,
    DWORD dwOptionalLength
    )
{
	WriteLog("cHttpSendRequestA");
	return HttpSendRequestA ( hRequest, (char *)lpszHeaders, dwHeadersLength, (char *)lpOptional, dwOptionalLength );
}


BOOL WINAPI cInternetReadFileExA(
    IN HINTERNET hFile,
    OUT LPINTERNET_BUFFERSA lpBuffersOut,
    IN DWORD dwFlags,
    IN DWORD dwContext
    )
{
	WriteLog ("cInternetReadFileEx()");
	char *iq = (char *)&InternetQueryDataAvailable;
	WriteLog ( "before hook = %d, %d, %d, %d, %d, addr=%lx", *(iq), *(iq+1), *(iq+2), *(iq+3), *(iq+4), iq );
	return InternetReadFileExA ( hFile, lpBuffersOut, dwFlags, dwContext );
}

BOOL WINAPI cInternetReadFile(
    IN HINTERNET hFile,
    IN LPVOID lpBuffer,
    IN DWORD dwNumberOfBytesToRead,
    OUT LPDWORD lpdwNumberOfBytesRead
    )
{
	WriteLog ("cInternetReadFile()");
	WriteLog ("iq=%lx\n", &InternetQueryDataAvailable);
	return InternetReadFile ( hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead );
}

BOOL WINAPI cInternetQueryDataAvailable(
    IN HINTERNET hFile,
    OUT LPDWORD lpdwNumberOfBytesAvailable OPTIONAL,
    IN DWORD dwFlags,
    IN DWORD dwContext
    )
{
	WriteLog ("QueryData()");
	return InternetQueryDataAvailable(hFile, lpdwNumberOfBytesAvailable, dwFlags, dwContext);
}


BOOL WINAPI cInternetWriteFile(
    IN HINTERNET hFile,
    IN LPCVOID lpBuffer,
    IN DWORD dwNumberOfBytesToWrite,
    OUT LPDWORD lpdwNumberOfBytesWritten
    )
{
	BOOL hResult = InternetWriteFile ( hFile, lpBuffer, dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten);
	return hResult;
}

BOOL WINAPI cInternetCloseHandle(
    IN HINTERNET hInternet
    )
{
	return InternetCloseHandle (hInternet);
}


MYAPIINFO myapi_info[] =
{
    {"WININET.DLL", "InternetConnectA", 8, "cInternetConnectA"},
		{"WININET.DLL", "HttpSendRequestA", 5, "cHttpSendRequestA"},
		{"WININET.DLL", "InternetWriteFile", 4, "cInternetWriteFile"},
		{"WININET.DLL", "InternetReadFileExA", 4, "cInternetReadFileExA"},
		{"WININET.DLL", "InternetReadFile", 4, "cInternetReadFile"},
		{"WININET.DLL", "InternetCloseHandle", 1, "cInternetCloseHandle"},
		{"WININET.DLL", "InternetQueryDataAvailable", 4, "cInternetQueryDataAvailable"},

    {NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}

