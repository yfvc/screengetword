// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// ------------------------------------- //

#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <lmcons.h>
#include <lmalert.h>
#include "mydll.h"
#include <nb30.h>
#include "util.h"
#include "filter.h"

char g_szDllPath[128];
HINSTANCE g_hInstance;
CSocketFilter socket_filter;

#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call =DLL_PROCESS_ATTACH)
	{
		GetProfileString("HookAPI", "dll_path", "", g_szDllPath, sizeof(g_szDllPath));
	}

	return TRUE;
}

int WINAPI mysocket(int af, int type, int protocol)
{
	WriteLog("mysocket, af=%d, type=%d, protocol=%d", af, type, protocol);

	return socket(af, type, protocol);
}

struct hostent * WINAPI mygethostbyname (const char * name)
{
	// filter IE url
	WriteLog("gethostbyname:name:%s", name);
	if(socket_filter.FilterDNS((char *)name))
	{
		WSASetLastError(-1);
		return NULL;
	}
	WriteLog("gethostbyname, do...");
	return gethostbyname(name);
}

int WINAPI myaccept(SOCKET s, struct sockaddr *pname, int *pnamelen)
{
	int port =GetLocalPortBySocket(s);

	WriteLog("myaccept");

	int s1 =accept(s, pname, pnamelen);

	if(s1 >0)
	{
		int err=WSAGetLastError();
		if(socket_filter.FilterAccept(s))
		{
			WSASetLastError(-1);
			return -1;
		}
	}

	return s1;
}

int WINAPI myconnect(SOCKET s, struct sockaddr *name, int namelen)
{
	struct sockaddr_in *paddr =(struct sockaddr_in *)name;

	char *ip =inet_ntoa(paddr->sin_addr);
	int port =ntohs(paddr->sin_port);
	WriteLog("connect: ip=%s, port=%d\n", ip, port);
	
	if(socket_filter.FilterConnect(ip, port))
	{
		WSASetLastError(-1);	
		return -1;
	}

	return connect(s, name, namelen);
}

int WINAPI myrecv(SOCKET s,	char *buf, int len, int flags)
{
	WriteLog("\nmyrecv");

	int recved_len =recv(s, (char *)buf, len, flags);
	int err;

	err =WSAGetLastError();

	if(recved_len >0)
	{
		socket_filter.FilterRecv(s, buf, len);
	}
	
	WSASetLastError(err);

	return recved_len;
}

int WINAPI mysend(SOCKET s,	char *buf, int len,	int flags)
{
	int ret;

	WriteLog("\nmysend");

	socket_filter.FilterSend(s, buf, len);
	ret =send(s, (char *)buf, len, flags);
	
	int err =WSAGetLastError();
	// do something
	WSASetLastError(err);
	
	return ret;
}

int WINAPI mysendto (SOCKET s, char FAR * buf, int len, int flags, struct sockaddr FAR * to, int tolen)
{
	WriteLog("mysendto");
	char *ip =inet_ntoa(((struct sockaddr_in *)to)->sin_addr);
	int port =ntohs(((struct sockaddr_in *)to)->sin_port);

	if(socket_filter.FilterConnect(ip, port))
	{
		WSASetLastError(-1);	
		return -1;
	}
	socket_filter.FilterSendTo(ip, port, buf, len);
	return sendto(s, buf, len, flags, to, tolen);
}

int WINAPI myrecvfrom (SOCKET s, char FAR * buf, int len, int flags, struct sockaddr FAR * from, int *fromlen)
{
	WriteLog("myrecvfrom");
	char *ip =inet_ntoa(((struct sockaddr_in *)from)->sin_addr);
	int port =ntohs(((struct sockaddr_in *)from)->sin_port);

	len = recvfrom(s, buf, len, flags, from, fromlen);
	
	int err=WSAGetLastError();
	socket_filter.FilterRecvFrom(ip, port, buf, len);
	WSASetLastError(err);

	return len;
}

HINSTANCE WINAPI myLoadLibraryA(char *name)
{
	char fname[128];
	GetModuleFileName(NULL, fname, sizeof(fname));
	WriteLog("%s,LoadLibraryA:%s", fname, name);
	return LoadLibraryA(name);
}

HINSTANCE WINAPI myLoadLibraryW(LPWSTR name)
{
	char fname[128];
	GetModuleFileName(NULL, fname, sizeof(fname));

	char aname[128];
	WideCharToMultiByte( CP_ACP, 0, name, -1, aname, 128,NULL,NULL); 
	WriteLog("%s,LoadLibraryW:%s", fname, aname);

	return LoadLibraryW(name);
}

FARPROC WINAPI myGetProcAddress(HMODULE hMod, char *name)
{
	char fname[128];
	GetModuleFileName(NULL, fname, sizeof(fname));
	if(strstr(fname, "HookAPI") ==NULL)
		WriteLog("%s,GetProcAddress:%s", fname, name);
	return GetProcAddress(hMod, name);
}

int WINAPI myNetbios(PNCB pncb)
{
	WriteLog("Netbios");

	return Netbios(pncb);
}

int WINAPI myNetAlertRaise(	LPWSTR event_name, LPVOID buf, DWORD buf_len)
{
	WriteLog("myNetAlertRaise...");
	return NetAlertRaise(event_name, buf, buf_len);
}

/*
int WINAPI myWSHOpenSocket(	int *family, int *type, int *protocol, LPWSTR device_name, LPVOID *context, LPDWORD event)
{
	WriteLog("WSHOpenSocket:event=%d", *event);

	HMODULE hMod=GetModuleHandle("WSHTcpip.dll");
	if(hMod ==NULL) return -1;
	WSHOPENSOCKET WSHOpenSocket =(WSHOPENSOCKET)GetProcAddress(hMod, "WSHOpenSocket");
	if(WSHOpenSocket ==NULL) return -1;

	return WSHOpenSocket(family, type, protocol, device_name, context,event);
}

int WINAPI myWSHOpenSocket2(int *family, int *type, int *protocol, int group, int flags, LPWSTR device_name, LPVOID *context, LPDWORD event)
{
	WriteLog("WSHOpenSocket2");

	HMODULE hMod=GetModuleHandle("WSHTcpip.dll");
	if(hMod ==NULL) return -1;
	WSHOPENSOCKET2 WSHOpenSocket2 =(WSHOPENSOCKET2)GetProcAddress(hMod, "WSHOpenSocket2");
	if(WSHOpenSocket2 ==NULL) return -1;

	return WSHOpenSocket2(family, type, protocol, group, flags, device_name, context,event);
}

int WINAPI myWSHNotify(	PVOID context, int sock, HANDLE ao, HANDLE co, DWORD event)
{
	
	HMODULE hMod=GetModuleHandle("WSHTcpip.dll");
	if(hMod ==NULL) return -1;
	WSHNOTIFY WSHNotify =(WSHNOTIFY)GetProcAddress(hMod, "WSHNotify");

	WriteLog("myWSHNotify:event=%d", event);

	return WSHNotify(context, sock, ao, co, event);
}
*/
// test hookprocess
int WINAPI myExitProcess(int exit_code)
{
	char mod_name[128];

	WriteLog("myExitProcess:exit_code=%d", exit_code);
	GetModuleFileName(NULL, mod_name, sizeof(mod_name));
	if(!strstr(mod_name, "hookprocess"))
	{
		ExitProcess(exit_code);
	}
	return 0;
}


DWORD WINAPI myCreateProcessW(
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
	char cmd[600];
	int len =WideCharToMultiByte( CP_ACP, 0, lpCommandLine, -1, cmd, 128,NULL,NULL); 
	cmd[len] =0;

	BOOL ifsuccess = CreateProcessW(lpApplicationName,
		lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	DWORD err =GetLastError();
	if(ifsuccess)
	{
		//HWND hWndNotify =FindWindow("HookAPINotify", NULL);
		//if(hWndNotify) PostMessage(hWndNotify, WM_APP+100, lpProcessInformation->dwProcessId, 0L);
		/*ResumeThread(lpProcessInformation->hThread);
		int ret=WaitForInputIdle(lpProcessInformation->hProcess, INFINITE);
		ResumeThread(lpProcessInformation->hThread);
		HANDLE hProcess=lpProcessInformation->hProcess;
		if((int)SuspendThread(lpProcessInformation->hThread) !=-1)
		{
		//if(is_nt)
		{
			char szdll[128];
			wsprintf(szdll, "%s\\mydll.dll", g_szDllPath);
		WriteLog("CreateProcessW: InjectLib %s", szdll);
			InjectLib(hProcess, szdll, TRUE);
			wsprintf(szdll, "%s\\HookAPINT.dll", g_szDllPath);
		WriteLog("CreateProcessW: InjectLib %s", szdll);
			InjectLib(hProcess, szdll, TRUE);
		}
		ResumeThread(lpProcessInformation->hThread);
		}
		WriteLog("CreateProcessW:%s, wait ret=%d, processid=%d", cmd, ret,lpProcessInformation->dwProcessId);
		*/
	}
	else WriteLog("CreateProcessW failed:%s", cmd);
	SetLastError(err);
	return (DWORD)ifsuccess;
}

DWORD WINAPI myCreateProcessA(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFO lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	WriteLog("myCreateProcessA, cmd=%s", lpCommandLine);

	BOOL ifsuccess = CreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	//WriteLog("CreateProcessA ok! ifsuccess=%d", ifsuccess);
	if(ifsuccess)
	{
		HANDLE hProcess =lpProcessInformation->hProcess;

		//if(is_nt)
		{
			//InjectLib(hProcess, "mydll.dll", TRUE);
			//InjectLib(hProcess, "HookAPINT.dll", TRUE);
		}
	}
	else
	{
		DWORD err;
		err =GetLastError();
		WriteLog("CreateProcessA failed:err=%d, cmd=%s\n", err, lpCommandLine);
		SetLastError(err);
	}
	return (DWORD)ifsuccess;
}

MYAPIINFO myapi_info[] =
{
	/*{"WSOCK32.DLL", "socket(a,b,c)", 3, "mysocket"},
	{"WSOCK32.DLL", "accept(a,b,c)", 3, "myaccept"},
	{"WSOCK32.DLL", "connect(SOCKET, struct sockaddr *, INT)", 3, "myconnect"},
	{"WSOCK32.DLL", "recv(INT, char *, INT, INT)", 4, "myrecv"},
	{"WSOCK32.DLL", "send(INT, char *, INT, INT)", 4, "mysend"},
	{"WSOCK32.DLL", "sendto(INT, char *, INT, INT, struct sockaddr *, int)", 6, "mysendto"},
	{"WSOCK32.DLL", "recvfrom(INT, char *, INT, INT, struct sockaddr *, int)", 6, "myrecvfrom"},
	{"WSOCK32.DLL", "gethostbyname(char *)", 1, "mygethostbyname"},
	*/
	{"WSOCK32.DLL", "socket", 3, "mysocket"},
	{"WSOCK32.DLL", "accept", 3, "myaccept"},
	{"WSOCK32.DLL", "connect", 3, "myconnect"},
	{"WSOCK32.DLL", "recv", 4, "myrecv"},
	{"WSOCK32.DLL", "send", 4, "mysend"},
	{"WSOCK32.DLL", "sendto", 6, "mysendto"},
	{"WSOCK32.DLL", "recvfrom", 6, "myrecvfrom"},
	{"WSOCK32.DLL", "gethostbyname", 1, "mygethostbyname"},
	//{"WSHTCPIP.DLL", "WSHOpenSocket(int *, int *, int *, LPWSTR, LPVOID *, LPDWORD)", "myWSHOpenSocket"},
	//{"WSHTCPIP.DLL", "WSHOpenSocket2(int *, int *, int *, int, int, LPWSTR, LPVOID *, LPDWORD)", "myWSHOpenSocket2"},
	//{"WSHTCPIP.DLL", "WSHNotify(PVOID, int, HANDLE, HANDLE, DWORD)", "myWSHNotify"},
	//{"NETAPI32.DLL", "Netbios(PNCB)", "myNetbios"},
	//{"KERNEL32.DLL", "LoadLibraryA(char *)", "myLoadLibraryA"},
	//{"KERNEL32.DLL", "ExitProcess(int)", "myExitProcess"},
	//{"KERNEL32.DLL", "CreateProcessA(1,2,3,4,5,6,7,8,9,10)", "myCreateProcessA"},
	//{"KERNEL32.DLL", "CreateProcessW(1,2,3,4,5,6,7,8,9,10)", "myCreateProcessW"},
	{NULL,NULL,0,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}

