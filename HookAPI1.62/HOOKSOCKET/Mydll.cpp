// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// 编译提示：如果是Win9x系统，需要预定义WIN95 //
// ------------------------------------- //

#include "stdafx.h"
#include <winsock.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <shellapi.h>
//#include <lmcons.h>
//#include <lmalert.h>
#include "../dll/mydll.h"
//#include <nb30.h>
#include <ras.h>
#include "util.h"

#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

HINSTANCE g_hInstance;

// 如果是win9x，不能使用fopen函数
int WriteBinData(char *function, char *buf, int len)
{
	char mod_name[100];
	char fname[128];

	if(len <=0) return 0;

	GetFileName(mod_name);
	wsprintf(fname, "c:\\%s.log", mod_name);
	HANDLE hFile;

	if((hFile =CreateFile(fname, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0)
	{
		WriteLog("open file %s failed", fname);
		return -1;
	}
	SetFilePointer(hFile, 0, NULL, FILE_END);
	char temp[2048];
	wsprintf(temp, "\r\n(%s,len=%d) ", function, len);
	DWORD dw;
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	for(int i =0; i<len; i++)
	{
		if(isgraph(buf[i]&0x00FF))
			wsprintf(temp, "%c", buf[i]&0x00FF);
		else
		    wsprintf(temp, "(%02x)", buf[i]&0x00FF);
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	}
	CloseHandle(hFile);

	return 0;
}

int GetLocalPortBySocket(SOCKET s)
{
	struct sockaddr name;
	int namelen =sizeof(name);

	getsockname(s, &name, &namelen);
	return ntohs(((struct sockaddr_in *)&name)->sin_port);
}

int GetLocalIPBySocket(SOCKET s, char *ip)
{
	struct sockaddr name;
	int namelen =sizeof(name);

	getsockname(s, &name, &namelen);
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&name)->sin_addr));
	return 0;
}

int GetRemotePortBySocket(SOCKET s)
{
	struct sockaddr name;
	int namelen =sizeof(name);

	getpeername(s, &name, &namelen);
	return ntohs(((struct sockaddr_in *)&name)->sin_port);
}

int GetIPAndPortByAddr(struct sockaddr *paddr, char *ip, int *port)
{
	*ip =0;
	*port =0;
	if(paddr ==NULL) return -1;

	strcpy(ip, inet_ntoa(((struct sockaddr_in *)paddr)->sin_addr));
	*port =ntohs(((struct sockaddr_in *)paddr)->sin_port);

	return 0;
}

int GetRemoteIPBySocket(SOCKET s, char *ip)
{
	struct sockaddr name;
	int namelen =sizeof(name);

	getpeername(s, &name, &namelen);
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&name)->sin_addr));
	return 0;
}

int WINAPI mysocket(int af, int type, int protocol)
{
	//Sleep(500);// test for multithread
	WriteLog("debug mysocket, af=%d, type=%d, protocol=%d", af, type, protocol);

	return socket(af, type, protocol);
}

struct hostent * WINAPI mygethostbyname (const char * name)
{
	// filter IE url
	//MessageBox(NULL, "mydll:mygethostbyname called", name, MB_OK);
	WriteLog("gethostbyname:name:%s", name);
	return gethostbyname(name);
}

int WINAPI myaccept(SOCKET s, struct sockaddr *pname, int *pnamelen)
{
	//WriteLog("myaccept");
	int port =GetLocalPortBySocket(s);

	int s1 =accept(s, pname, pnamelen);

	if(s1 >0)
	{
		// check filter 
	}

	return s1;
}

int WINAPI myconnect(SOCKET s, struct sockaddr *name, int namelen)
{
	WriteLog("myconnect");
	struct sockaddr_in *paddr =(struct sockaddr_in *)name;

	char *ip =inet_ntoa(paddr->sin_addr);
	int port =ntohs(paddr->sin_port);
	WriteLog("connect: ip=%s, port=%d\r\n", ip, port);
	char temp[50];
	wsprintf(temp, "ip=%s, port=%d\r\n", ip, port);
	WriteBinData("connect", temp, strlen(temp));
	BYTE *p =(BYTE *)GetProcAddress(GetModuleHandle("ws2_32.dll"), "recv");
	if(p)
	{
		sprintf(temp, "my connect: data of recv=%x:%x %x %x %x %x", p, p[0], p[1],p[2],p[3],p[4]);
		WriteLog(temp);
	}

	// check filter
	return connect(s, name, namelen);
}

int WINAPI myrecv(SOCKET s,	char *buf, int len, int flags)
{
	WriteLog("myrecv, len:%d", len);

	int recved_len =recv(s, (char *)buf, len, flags);
	int err;

	if(recved_len <=0) err =WSAGetLastError();

	int port =GetLocalPortBySocket(s);
	int port1 =GetRemotePortBySocket(s);
	char ip[16];
	GetRemoteIPBySocket(s, ip);


	char temp[200];
	wsprintf(temp, "ip:%s, local_port:%d, remote_port:%d", ip, port, port1);

	WriteBinData("recv", temp, strlen(temp));
	WriteBinData("recv", buf, recved_len);

	BYTE *p =(BYTE *)GetProcAddress(GetModuleHandle("ws2_32.dll"), "recv");
	if(p)
	{
		sprintf(temp, "myrecv: data of recv=%x:%x %x %x %x %x", p, p[0], p[1],p[2],p[3],p[4]);
		WriteLog(temp);
	}

	if(recved_len <=0) WSASetLastError(err);
	
	return recved_len;
}

int WINAPI mysend(SOCKET s,	char *buf, int len,	int flags)
{
	int ret;

	WriteLog("mysend, len:%d", len);

	int port =GetLocalPortBySocket(s);
	int port1 =GetRemotePortBySocket(s);
	char ip[16];

	GetRemoteIPBySocket(s, ip);

	char temp[200];
	wsprintf(temp, "ip:%s, local_port:%d, remote_port:%d", ip, port, port1);

	WriteBinData("send", temp, strlen(temp));
	WriteBinData("send", buf, len);

	BYTE *p =(BYTE *)GetProcAddress(GetModuleHandle("ws2_32.dll"), "recv");
	if(p)
	{
		sprintf(temp, "mysend data of recv=%x:%x %x %x %x %x", p, p[0], p[1],p[2],p[3],p[4]);
		WriteLog(temp);
	}

	ret =send(s, (char *)buf, len, flags);
	
	int err;
	if(ret <=0) err =WSAGetLastError();
	// other process...
	if(ret <=0) WSASetLastError(err);
	
	return ret;
}

int WINAPI mysendto (SOCKET s, char FAR * buf, int len, int flags, struct sockaddr FAR * to, int tolen)
{
	WriteLog("mysendto");
	int port;
	char ip[16];
	char temp[200];

	GetIPAndPortByAddr(to, ip, &port);
	sprintf(temp, "ip:%s, remote_port:%d", ip, port);

	WriteBinData("sendto", temp, strlen(temp));
	WriteBinData("sendto", buf, len);
	BYTE *p =(BYTE *)GetProcAddress(GetModuleHandle("ws2_32.dll"), "recv");
	if(p)
	{
		sprintf(temp, "data of recv=%x:%x %x %x %x %x", p, p[0], p[1],p[2],p[3],p[4]);
		WriteLog(temp);
	}

	return sendto(s, buf, len, flags, to, tolen);
}

int WINAPI myrecvfrom (SOCKET s, char FAR * buf, int len, int flags, struct sockaddr FAR * from, int *fromlen)
{
	WriteLog("myrecvfrom");
	int ret = recvfrom(s, buf, len, flags, from, fromlen);
	int err=WSAGetLastError();
	
	int port;
	char ip[16];
	char temp[200];

	GetIPAndPortByAddr(from, ip, &port);
	sprintf(temp, "ip:%s, remote_port:%d, len=%d", ip, port, len);

	WriteBinData("recvfrom", temp, strlen(temp));
	WriteBinData("recvfrom", buf, ret);

	WSASetLastError(err);

	return ret;
}
/*
HINSTANCE WINAPI myShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile,
							 LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	WriteLog("ShellExecuteW file=%s", lpFile);
	return ShellExecuteA(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}
HINSTANCE WINAPI myShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd)
{
	char fname[400];
	int len =WideCharToMultiByte( CP_ACP, 0, lpFile, -1, fname, sizeof(fname),NULL,NULL);
	fname[len] =0;
	WriteLog("ShellExecuteW file=%s", fname);
	return ShellExecuteW(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

BOOL WINAPI myShellExecuteExA(LPSHELLEXECUTEINFOA lpExecInfo)
{
	WriteLog("ShellExecuteExA");
	return ShellExecuteExA(lpExecInfo);
}

BOOL WINAPI myShellExecuteExW(LPSHELLEXECUTEINFOW lpExecInfo)
{
	WriteLog("ShellExecuteExW");
	return ShellExecuteExW(lpExecInfo);
}

BOOL WINAPI myCreateProcessAsUserA (
    HANDLE hToken,
    LPCSTR lpApplicationName,
    LPSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCSTR lpCurrentDirectory,
    LPSTARTUPINFOA lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    )
{
	BOOL bbb= CreateProcessAsUserA (hToken, lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation);
	WriteLog("CreateProcessAsUserA");
	return bbb;
}

BOOL WINAPI myCreateProcessAsUserW (
    HANDLE hToken,
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    )
{
	BOOL bbb = CreateProcessAsUserW (hToken, lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation);
	WriteLog("CreateProcessAsUserW");
	return bbb;
}
*/

// 地址小的放前面
MYAPIINFO myapi_info[] =
{
	// "kernel32.dll" CreateProcessInternalA
	//{"advapi32.dll", "CreateProcessAsUserA", 11, "myCreateProcessAsUserA"},
	//{"advapi32.dll", "CreateProcessAsUserW", 12, "myCreateProcessAsUserW"},
	//{"Shell32.dll", "ShellExecuteA", 6, "myShellExecuteA"},
	//{"Shell32.dll", "ShellExecuteW", 6, "myShellExecuteW"},
	//{"Shell32.dll", "ShellExecuteExA", 1, "myShellExecuteExA"},
	//{"Shell32.dll", "ShellExecuteExW", 1, "myShellExecuteExW"},
#ifdef WINNT
	{"WS2_32.DLL", "socket", 3, "mysocket"},
	{"WS2_32.DLL", "accept", 3, "myaccept"},
	{"WS2_32.DLL", "connect", 3, "myconnect"},
	{"WS2_32.DLL", "recv", 4, "myrecv"},
	{"WS2_32.DLL", "send", 4, "mysend"},
	{"WS2_32.DLL", "sendto", 6, "mysendto"},
	{"WS2_32.DLL", "recvfrom", 6, "myrecvfrom"},
	{"WS2_32.DLL", "gethostbyname", 1, "mygethostbyname"},
#else
	{"WSOCK32.DLL", "socket", 3, "mysocket"},
	{"WSOCK32.DLL", "accept", 3, "myaccept"},
	{"WSOCK32.DLL", "connect", 3, "myconnect"},
	{"WSOCK32.DLL", "recv", 4, "myrecv"},
	{"WSOCK32.DLL", "send", 4, "mysend"},
	{"WSOCK32.DLL", "sendto", 6, "mysendto"},
	{"WSOCK32.DLL", "recvfrom", 6, "myrecvfrom"},
	{"WSOCK32.DLL", "gethostbyname", 1, "mygethostbyname"},
#endif
	{NULL}
};

// 下列内容请不要修改
MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call =DLL_PROCESS_ATTACH)
	{
		//GetProfileString("HookAPI", "dll_path", "", g_szDllPath, sizeof(g_szDllPath));
	}

	return TRUE;
}

