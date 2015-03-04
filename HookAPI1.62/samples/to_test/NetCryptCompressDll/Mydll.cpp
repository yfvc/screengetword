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
#include "const.h"
#include "cryptfilter.h"

CryptFilter g_CryptFilter;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetProfileString("HookAPI", "dll_path", "", g_szDllPath, sizeof(g_szDllPath));
		GetFileName(g_szAppBaseName);
		strupr(g_szAppBaseName);
		break;
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
	return gethostbyname(name);
}

int WINAPI myaccept(SOCKET s, struct sockaddr *pname, int *pnamelen)
{
	int port =GetLocalPortBySocket(s);

	WriteLog("myaccept");

	int s1 =accept(s, pname, pnamelen);

	if(s1 >0)
	{
		int ov, len =sizeof(ov);
		getsockopt(s1, SOL_SOCKET, SO_ACCEPTCONN, (char *)&ov, &len);
		WriteLog("accept...%d", ov); 
	}

	return s1;
}

int WINAPI myconnect(SOCKET s, struct sockaddr *name, int namelen)
{
	struct sockaddr_in *paddr =(struct sockaddr_in *)name;

	char *ip =inet_ntoa(paddr->sin_addr);
	int port =ntohs(paddr->sin_port);
	WriteLog("connect: ip=%s, port=%d\n", ip, port);
	// check filter
	return connect(s, name, namelen);
}

// 这种mysend和这种myrecv支持异步方式
int WINAPI myrecv(SOCKET s,	char *buf, int len, int flags)
{
	WriteLog("\nmyrecv");

	int f_crypted;
	int port =GetLocalPortBySocket(s);
	int port1 =GetRemotePortBySocket(s);
	char ip[16];
	GetRemoteIPBySocket(s, ip);
	char *buf1 =NULL;
	int recved_len;
	int ret;
	int err;

	f_crypted =g_CryptFilter.CryptCheck(ip, port, port1);
	if(f_crypted)
	{
		buf1 =(char *)malloc(len*2);
		int len2;
		if(recv(s, (char *)&len2, sizeof(len2), flags) !=sizeof(len2))
		{
			err =WSAGetLastError();
			free(buf1);
			WSASetLastError(err);
			return -1;
		}
		len =ntohl(len2);
		WriteLog("myrecv:len=%d", len);
		int len1;
		time_t t1, t2;
		time(&t1);
		t2 =t1;
		recved_len =0;
		while(t2-t1 <10 && recved_len <len)
		{
			len2 =recv(s, (char *)buf, len-recved_len, flags);
			if(len2 >0)
				recved_len +=len2;
			time(&t2);
		}
		err =WSAGetLastError();
		WriteLog("recved_len:%d", recved_len);
		if(recved_len <=0)
		{
			free(buf1);
			WSASetLastError(err);
			return -1;
		}
		WriteLog("DecryptData...");
		ret =g_CryptFilter.DecryptData(buf, recved_len, buf1, &len1);
		if(ret >0)
		{
			memcpy(buf, buf1, len1);
			recved_len =len1;
		}
		else if(ret <0)
		{
			free(buf1);
			WSASetLastError(-1);
			return -1;
		}
		WriteLog("DecryptData ret:%d", ret);
		free(buf1);
		WSASetLastError(err);
	}
	else
		recved_len =recv(s, (char *)buf, len, flags);

	return recved_len;
}

int WINAPI mysend(SOCKET s,	char *buf, int len,	int flags)
{
	WriteLog("\nmysend");

	int f_crypted;
	int port =GetLocalPortBySocket(s);
	int port1 =GetRemotePortBySocket(s);
	char ip[16];
	GetRemoteIPBySocket(s, ip);
	char *buf1 =NULL;
	int ret, ret_len =len;

	f_crypted =g_CryptFilter.CryptCheck(ip, port, port1);
	if(f_crypted)
	{
		buf1 =(char *)malloc(len*2);
		int len1;
		WriteLog("CryptData...");
		ret =g_CryptFilter.CryptData(buf, len, buf1, &len1);
		if(ret >0)
		{
			memcpy(buf, buf1, len1);
			len =len1;
		}
		else if(ret <0)
		{
			free(buf1);
			WSASetLastError(-1);
			return -1;
		}
		WriteLog("Crypt ret:%d", ret);
		free(buf1);
		int len2 =htonl(len);
		if(send(s, (char *)&len2, sizeof(len2), flags) !=sizeof(len2))
			return -1;
		WriteLog("send len=%d", len);
	}

	if(send(s, (char *)buf, len, flags) !=len)
		ret_len =-1;
	return ret_len;
}

int WINAPI mysendto (SOCKET s, char FAR * buf, int len, int flags, struct sockaddr FAR * to, int tolen)
{
	WriteLog("mysendto");
	return sendto(s, buf, len, flags, to, tolen);
}

int WINAPI myrecvfrom (SOCKET s, char FAR * buf, int len, int flags, struct sockaddr FAR * from, int *fromlen)
{
	WriteLog("mysendto");
	return recvfrom(s, buf, len, flags, from, fromlen);
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
	{"WSOCK32.DLL", "socket(a,b,c)", "mysocket"},
	{"WSOCK32.DLL", "accept(a,b,c)", "myaccept"},
	{"WSOCK32.DLL", "connect(SOCKET, struct sockaddr *, INT)", "myconnect"},
	{"WSOCK32.DLL", "recv(INT, char *, INT, INT)", "myrecv"},
	{"WSOCK32.DLL", "send(INT, char *, INT, INT)", "mysend"},
	{"WSOCK32.DLL", "sendto(INT, char *, INT, INT, struct sockaddr *, int)", "mysendto"},
	{"WSOCK32.DLL", "recvfrom(INT, char *, INT, INT, struct sockaddr *, int)", "myrecvfrom"},
	{"WSOCK32.DLL", "gethostbyname(char *)", "mygethostbyname"},
	//{"WSHTCPIP.DLL", "WSHOpenSocket(int *, int *, int *, LPWSTR, LPVOID *, LPDWORD)", "myWSHOpenSocket"},
	//{"WSHTCPIP.DLL", "WSHOpenSocket2(int *, int *, int *, int, int, LPWSTR, LPVOID *, LPDWORD)", "myWSHOpenSocket2"},
	//{"WSHTCPIP.DLL", "WSHNotify(PVOID, int, HANDLE, HANDLE, DWORD)", "myWSHNotify"},
	//{"NETAPI32.DLL", "Netbios(PNCB)", "myNetbios"},
	//{"KERNEL32.DLL", "LoadLibraryA(char *)", "myLoadLibraryA"},
	//{"KERNEL32.DLL", "ExitProcess(int)", "myExitProcess"},
	//{"KERNEL32.DLL", "CreateProcessA(1,2,3,4,5,6,7,8,9,10)", "myCreateProcessA"},
	//{"KERNEL32.DLL", "CreateProcessW(1,2,3,4,5,6,7,8,9,10)", "myCreateProcessW"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}

