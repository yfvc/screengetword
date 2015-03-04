// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI 开发例子          //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// ------------------------------------- //

#include "stdafx.h"
#include <stdio.h>

#include "mydll.h"
#include "util.h"
#include "psapi.h"
#include "tlhelp32.h"
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "ps.h"

HINSTANCE g_hInstance =NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance =(HINSTANCE)hModule;

    return TRUE;
}

LPITEMIDLIST WINAPI mySHBrowseForFolderA(BROWSEINFOA *pbi)
{
	WriteLog("mySHBrowseForFolderA");

	return SHBrowseForFolderA(pbi);
}

LPITEMIDLIST WINAPI mySHBrowseForFolderW(BROWSEINFOW *pbi)
{          
	WriteLog("mySHBrowseForFolderW");
	return SHBrowseForFolderW(pbi);
}

HRESULT WINAPI mySHGetDataFromIDListA( LPSHELLFOLDER psf, LPCITEMIDLIST pidl, int nFormat, PVOID pv, int cb)
{
	WriteLog("myShGetDataFromIDListA");

	return SHGetDataFromIDListA( psf, pidl, nFormat, pv, cb);
}

HRESULT WINAPI mySHGetDataFromIDListW( LPSHELLFOLDER psf, LPCITEMIDLIST pidl, int nFormat, PVOID pv, int cb)
{
	WriteLog("myShGetDataFromIDListW");
	return 0;
	//return SHGetDataFromIDListW( psf, pidl, nFormat, pv, cb);
}

HRESULT WINAPI mySHGetDesktopFolder( LPSHELLFOLDER *ppshf)
{
	//WriteLog("myShGetDesktopFolder");
	return SHGetDesktopFolder(ppshf);
}

HRESULT WINAPI mySHGetSpecialFolderLocation(HWND hwndOwner, int nFolder, LPITEMIDLIST *ppidl)
{
	WriteLog("mySHGetSpecialFolderLocation,nFolder=%d", nFolder);
	//return 0;
	return SHGetSpecialFolderLocation(hwndOwner, nFolder, ppidl);
}

BOOL WINAPI mySHGetPathFromIDListA( LPCITEMIDLIST pidl, LPSTR pszPath)
{
	WriteLog("mySHGetPathFromIDListA");

	return SHGetPathFromIDListA(pidl, pszPath);
}

BOOL WINAPI mySHGetPathFromIDListW( LPCITEMIDLIST pidl, LPWSTR pszPath)
{
	WriteLog("mySHGetPathFromIDListW");
	return 0;
	//return SHGetPathFromIDListW(pidl, pszPath);
}

DWORD WINAPI mySHGetFileInfoA( LPCSTR pszPath, DWORD dwFileAttributes, SHFILEINFOA FAR *psfi, UINT cbFileInfo, UINT uFlags)
{
	WriteLog("mySHGetFileInfoA");

	return SHGetFileInfoA(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
}

DWORD WINAPI mySHGetFileInfoW( LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW FAR *psfi, UINT cbFileInfo, UINT uFlags)
{
	char path[128];
	int len =WideCharToMultiByte(CP_ACP, 0, pszPath, -1, path, sizeof(path), NULL, NULL);
	path[len] =0;
	WriteLog("mySHGetFileInfoW:%s", path);
	//return 0;
	return SHGetFileInfoW(pszPath, dwFileAttributes, psfi, cbFileInfo, uFlags);
}

typedef HRESULT (WINAPI *FuncSHGetFolderPathA)(HWND, int, HANDLE, DWORD, LPSTR);
typedef HRESULT (WINAPI *FuncSHGetFolderPathW)(HWND, int, HANDLE, DWORD, LPWSTR);

HRESULT WINAPI mySHGetFolderPathA( HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPSTR pszPath)
{
	WriteLog("SHGetFolderPathA");
	HMODULE hmod =GetModuleHandle("shell32.dll");
	FuncSHGetFolderPathA SHGetFolderPathA=(FuncSHGetFolderPathA)GetProcAddress(hmod, "SHGetFolderPathA");
	if(SHGetFolderPathA)
		return SHGetFolderPathA( hwndOwner, nFolder, hToken, dwFlags, pszPath); 
	else return 0;
}

HRESULT WINAPI mySHGetFolderPathW( HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath)
{

	HMODULE hmod =GetModuleHandle("shell32.dll");
	FuncSHGetFolderPathW SHGetFolderPathW=(FuncSHGetFolderPathW)GetProcAddress(hmod, "SHGetFolderPathW");
	if(SHGetFolderPathW)
	{
		HRESULT l=SHGetFolderPathW( hwndOwner, nFolder, hToken, dwFlags, pszPath); 
		char path[128];
		int len =WideCharToMultiByte(CP_ACP, 0, pszPath, -1, path, sizeof(path), NULL, NULL);
		path[len] =0;
		WriteLog("SHGetFolderPathW:%s", path);
		return l;
	}
	else
		return 0;
}

typedef HRESULT (WINAPI *FuncSHGetFolderLocation)(HWND, int, HANDLE, DWORD, LPITEMIDLIST*);

HRESULT WINAPI mySHGetFolderLocation( HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwReserved, LPITEMIDLIST* ppidl)
{
	WriteLog("mySHGetFolderLocation");

	HMODULE hmod =GetModuleHandle("shell32.dll");
	FuncSHGetFolderLocation SHGetFolderLocation=(FuncSHGetFolderLocation)GetProcAddress(hmod, "SHGetFolderLocation");
	if(SHGetFolderLocation)
		return SHGetFolderLocation( hwndOwner, nFolder, hToken, dwReserved, ppidl); 
	else
		return 0;
}

BOOL WINAPI myPathFileExistsW(LPCWSTR pstr)
{
	char path[128];
	int len =WideCharToMultiByte(CP_ACP, 0, pstr, -1, path, sizeof(path), NULL, NULL);
	path[len] =0;

	WriteLog("PathFileExistsW:%s", path);
	return PathFileExistsW(pstr);
}

typedef int (WINAPI *FuncSheChangeDirW)(LPCWSTR);

int WINAPI mySheChangeDirW(LPCWSTR pstr)
{
	char path[128];
	int len =WideCharToMultiByte(CP_ACP, 0, pstr, -1, path, sizeof(path), NULL, NULL);
	path[len] =0;

	WriteLog("SheChangeDirW:%s", path);
	HMODULE hmod =GetModuleHandle("shell32.dll");
	FuncSheChangeDirW SheChangeDirW=(FuncSheChangeDirW)GetProcAddress(hmod, "SheChangeDirW");
	if(SheChangeDirW)
		return SheChangeDirW(pstr); 
	else
		return 0;
}

MYAPIINFO myapi_info[] =
{
	{"SHELL32.DLL", "SHBrowseForFolderA", 1, "mySHBrowseForFolderA"},
	{"SHELL32.DLL", "SHBrowseForFolderW", 1, "mySHBrowseForFolderW"},
	{"SHELL32.DLL", "SHGetDataFromIDListA", 5, "mySHGetDataFromIDListA"},
	{"SHELL32.DLL", "SHGetDataFromIDListW", 5, "mySHGetDataFromIDListW"},
	{"SHELL32.DLL", "SHGetDesktopFolder", 1, "mySHGetDesktopFolder"},
	{"SHELL32.DLL", "SHGetSpecialFolderLocation", 3, "mySHGetSpecialFolderLocation"},
	{"SHELL32.DLL", "SHGetPathFromIDListA", 2, "mySHGetPathFromIDListA"},
	{"SHELL32.DLL", "SHGetPathFromIDListW", 2, "mySHGetPathFromIDListW"},
	{"SHELL32.DLL", "SHGetFileInfoA", 5, "mySHGetFileInfoA"},
	{"SHELL32.DLL", "SHGetFileInfoW", 5, "mySHGetFileInfoW"},
	{"SHELL32.DLL", "SHGetFolderPathA", 5, "mySHGetFolderPathA"},
	{"SHELL32.DLL", "SHGetFolderPathW", 5, "mySHGetFolderPathW"},
	{"SHELL32.DLL", "SHGetFolderLocation", 5,"mySHGetFolderLocation"},
	{"SHLWAPI.DLL", "PathFileExistsW", 1, "myPathFileExistsW"},
	{"SHELL32.DLL", "SheChangeDirW", 1, "mySheChangeDirW"},
	{NULL,NULL,NULL}
};

MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}
