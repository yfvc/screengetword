/*
  此文件必须使用release方式编译才能正确运行
*/

#include <windows.h>
#include <stdio.h>
#include "injlib.h"
#include "util.h"

typedef HINSTANCE (WINAPI *PROCLOADLIBRARY)(LPSTR);
typedef BOOL (WINAPI *PROCFREELIBRARY)(HINSTANCE);
typedef HMODULE (WINAPI *PROCGETMODULEHANDLE)(LPSTR);

void WriteLog(char *fmt,...);

#define LOAD_DLL	1
#define FREE_DLL	0

typedef struct {
   PROCLOADLIBRARY fnLoadLibrary;
   PROCFREELIBRARY fnFreeLibrary;
   PROCGETMODULEHANDLE fnGetModuleHandle;
   BOOL ActionFlag;
   CHAR pbLibFile[MAX_PATH];
} INJLIBINFO, *PINJLIBINFO;

// Calls to the stack checking routine must be disabled.
#pragma check_stack (off)

static DWORD WINAPI ThreadFunc (PINJLIBINFO pInjLibInfo) {
   // There must be less than a page worth of local
   // variables used in this function.
   HINSTANCE hinstDll =NULL;

   if(pInjLibInfo ==NULL || pInjLibInfo->fnGetModuleHandle ==NULL
		|| pInjLibInfo->fnLoadLibrary ==NULL) return (DWORD)NULL;

   hinstDll = pInjLibInfo->fnGetModuleHandle(pInjLibInfo->pbLibFile);

   if (pInjLibInfo->ActionFlag == LOAD_DLL)
   {
     if (hinstDll == NULL)
       hinstDll = pInjLibInfo->fnLoadLibrary(pInjLibInfo->pbLibFile);
   }
   else if (pInjLibInfo->ActionFlag == FREE_DLL)
     pInjLibInfo->fnFreeLibrary(hinstDll);

   // The thread's exit code is the handle of the DLL.

   return((DWORD) hinstDll);
}


//////////////////////////////////////////////////////////////


// This function marks the memory address after ThreadFunc.
// ThreadFuncCodeSizeInBytes =
//    (PBYTE) AfterThreadFunc - (PBYTE) ThreadFunc.
static void AfterThreadFunc (void) {
}
#pragma check_stack 


//////////////////////////////////////////////////////////////


int WINAPI InjectLib (DWORD process_id, char *pbLibFile)
{
   // Kernel32.DLL's HINSTANCE is used to get the
   // address of LoadLibraryA or LoadLibraryW and
   // FreeLibrary.
   HINSTANCE hinstKrnl = GetModuleHandle("Kernel32");

   INJLIBINFO InjLibInfo;

   // The address to which code will be copied in the
   // remote process
   PDWORD pdwCodeRemote = NULL;

   // Calculate the number of bytes in the ThreadFunc
   // function.
#ifndef _PPC_
   const int cbCodeSize = ((LPBYTE) (DWORD)
      AfterThreadFunc - (LPBYTE) (DWORD) ThreadFunc);
#else
   // Note: The PowerPC does not support 32-bit immediate
   // values. So, the VC++ PowerPC compiler implements 32-bit
   // immediates using an offset into a table and grabbing
   // the 32-bit value from the table. The references to
   // the functions below, are actually offsets into this
   // table. So, we have to subtract the contents of these
   // offsets to get the actual code size.
   const int cbCodeSize = ((LPBYTE) *(PDWORD)
      AfterThreadFunc - (LPBYTE) *(PDWORD) ThreadFunc);
#endif

   // The address to which INJLIBINFO will be copied in
   // the remote process
   PINJLIBINFO pInjLibInfoRemote = NULL;
   // The number of bytes written to the remote process
   DWORD dwNumBytesXferred = 0;

   // The handle and ID of the thread executing the
   // remote copy of ThreadFunc
   DWORD dwThreadId = 0;
   HANDLE hThread = NULL;
   HINSTANCE hinstDllRemote = NULL;

   BOOL fOk = FALSE;

   // Initialize the INJLIBINFO structure here, and
   // then copy it to memory in the remote process.
   InjLibInfo.fnLoadLibrary = (PROCLOADLIBRARY)GetProcAddress(hinstKrnl, "LoadLibraryA");
   InjLibInfo.fnFreeLibrary = (PROCFREELIBRARY)GetProcAddress(hinstKrnl, "FreeLibrary");
   InjLibInfo.fnGetModuleHandle = (PROCGETMODULEHANDLE)GetProcAddress(hinstKrnl, "GetModuleHandleA");

   InjLibInfo.ActionFlag = LOAD_DLL;

   InjLibInfo.pbLibFile[0] = 0;   // Initialized later

      strcpy(InjLibInfo.pbLibFile, pbLibFile);

    HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess ==NULL)
	{
		WriteLog("InjectLib:OpenProcess %d failed!", process_id);
		return -1;
	}


	if((pdwCodeRemote = (PDWORD)VirtualAllocEx(hProcess, NULL, cbCodeSize+20, MEM_COMMIT, PAGE_EXECUTE_READWRITE)) ==NULL)
	  {
 		CloseHandle(hProcess);
          return FALSE;
	  }

      // Change the page protection of the allocated memory
      // to executable, read, and write.
	  int ret;
      ret =WriteProcessMemory(hProcess, pdwCodeRemote,
         (LPVOID)(DWORD) ThreadFunc, cbCodeSize,
         &dwNumBytesXferred);
	  //WriteLog("WriteProcessMemory pdwCodeRemote size:%d, ret=%d, %d", cbCodeSize, ret, STATUS_ACCESS_VIOLATION);
	  if(ret ==STATUS_ACCESS_VIOLATION || ret ==false)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
 		CloseHandle(hProcess);
		  return false;
	  }


      // Put InjLibInfo in the remote thread's memory block.
      if((pInjLibInfoRemote = (INJLIBINFO *)VirtualAllocEx(hProcess, NULL, sizeof(InjLibInfo)+20, MEM_COMMIT, PAGE_READWRITE)) ==NULL)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
 		CloseHandle(hProcess);
         return FALSE;
	  }
      ret =WriteProcessMemory(hProcess, pInjLibInfoRemote,
         &InjLibInfo, sizeof(InjLibInfo), &dwNumBytesXferred);
	  //WriteLog("WriteProcessMemory InjLibInfo ret=%d", ret);
	  if(ret ==STATUS_ACCESS_VIOLATION || ret ==false)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
		  VirtualFreeEx(hProcess, pInjLibInfoRemote, 0, MEM_RELEASE);
 		CloseHandle(hProcess);
		  return false;
	  }
	  //WriteLog("CreateRemoteProcess...");
      hThread = CreateRemoteThread(hProcess, NULL, 0, 
         (LPTHREAD_START_ROUTINE)(DWORD) pdwCodeRemote,
         pInjLibInfoRemote, 0, &dwThreadId);
      if (hThread == NULL)
	  {
		  //WriteLog("CreateRemoteProcess failed");
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
		  VirtualFreeEx(hProcess, pInjLibInfoRemote, 0, MEM_RELEASE);
 		CloseHandle(hProcess);
		  return false;
	  }
	  //WriteLog("CreateRemoteThread success, WaitForSingleObject...");
      WaitForSingleObject(hThread, INFINITE);

      if (hThread != NULL)
	  {
         GetExitCodeThread(hThread, (PDWORD) &hinstDllRemote);
         CloseHandle(hThread);
      }
      // Let the remote thread start executing the remote 
      // ThreadFunc function using our modified stack, which
      // now contains an initialized INJLIBINFO structure.
      if(pdwCodeRemote)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
	  }
      if(pInjLibInfoRemote)
	  {
		  VirtualFreeEx(hProcess, pInjLibInfoRemote, 0, MEM_RELEASE);
	  }
	//WriteLog("OK++++");
 		CloseHandle(hProcess);
	return(hinstDllRemote != NULL);
}

int WINAPI EjectLib (DWORD process_id, char *pbLibFile)
{
   // Kernel32.DLL's HINSTANCE is used to get the
   // address of LoadLibraryA or LoadLibraryW and
   // FreeLibrary.
   HINSTANCE hinstKrnl = GetModuleHandle("Kernel32");

   INJLIBINFO InjLibInfo;

   // The address to which code will be copied in the
   // remote process
   PDWORD pdwCodeRemote = NULL;

   // Calculate the number of bytes in the ThreadFunc
   // function.
#ifndef _PPC_
   const int cbCodeSize = ((LPBYTE) (DWORD)
      AfterThreadFunc - (LPBYTE) (DWORD) ThreadFunc);
#else
   // Note: The PowerPC does not support 32-bit immediate
   // values. So, the VC++ PowerPC compiler implements 32-bit
   // immediates using an offset into a table and grabbing
   // the 32-bit value from the table. The references to
   // the functions below, are actually offsets into this
   // table. So, we have to subtract the contents of these
   // offsets to get the actual code size.
   const int cbCodeSize = ((LPBYTE) *(PDWORD)
      AfterThreadFunc - (LPBYTE) *(PDWORD) ThreadFunc);
#endif

   // The address to which INJLIBINFO will be copied in
   // the remote process
   PINJLIBINFO pInjLibInfoRemote = NULL;
   // The number of bytes written to the remote process
   DWORD dwNumBytesXferred = 0;

   // The handle and ID of the thread executing the
   // remote copy of ThreadFunc
   DWORD dwThreadId = 0;
   HANDLE hThread = NULL;
   HINSTANCE hinstDllRemote = NULL;

   BOOL fOk = FALSE;

   // Initialize the INJLIBINFO structure here, and
   // then copy it to memory in the remote process.
   InjLibInfo.fnLoadLibrary = (PROCLOADLIBRARY)GetProcAddress(hinstKrnl, "LoadLibraryA");
   InjLibInfo.fnFreeLibrary = (PROCFREELIBRARY)GetProcAddress(hinstKrnl, "FreeLibrary");
   InjLibInfo.fnGetModuleHandle = (PROCGETMODULEHANDLE)GetProcAddress(hinstKrnl, "GetModuleHandleA");

   InjLibInfo.ActionFlag = FREE_DLL;

   InjLibInfo.pbLibFile[0] = 0;   // Initialized later

      strcpy(InjLibInfo.pbLibFile, pbLibFile);

 	HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess ==NULL)
	{
		WriteLog("InjectLib:OpenProcess %d failed!", process_id);
		return -1;
	}

	if((pdwCodeRemote = (PDWORD)VirtualAllocEx(hProcess, NULL, cbCodeSize+20, MEM_COMMIT, PAGE_EXECUTE_READWRITE)) ==NULL)
	  {
 		CloseHandle(hProcess);
         return FALSE;
	  }

      // Change the page protection of the allocated memory
      // to executable, read, and write.
	  int ret;
      ret =WriteProcessMemory(hProcess, pdwCodeRemote,
         (LPVOID)(DWORD) ThreadFunc, cbCodeSize,
         &dwNumBytesXferred);
	  //WriteLog("WriteProcessMemory pdwCodeRemote size:%d, ret=%d, %d", cbCodeSize, ret, STATUS_ACCESS_VIOLATION);
	  if(ret ==STATUS_ACCESS_VIOLATION || ret ==false)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		  return false;
	  }


      // Put InjLibInfo in the remote thread's memory block.
      if((pInjLibInfoRemote = (INJLIBINFO *)VirtualAllocEx(hProcess, NULL, sizeof(InjLibInfo)+20, MEM_COMMIT, PAGE_READWRITE)) ==NULL)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
		CloseHandle(hProcess);
         return FALSE;
	  }
      ret =WriteProcessMemory(hProcess, pInjLibInfoRemote,
         &InjLibInfo, sizeof(InjLibInfo), &dwNumBytesXferred);
	  //WriteLog("WriteProcessMemory InjLibInfo ret=%d", ret);
	  if(ret ==STATUS_ACCESS_VIOLATION || ret ==false)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
		  VirtualFreeEx(hProcess, pInjLibInfoRemote, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		  return false;
	  }
	  //WriteLog("CreateRemoteProcess...");
      hThread = CreateRemoteThread(hProcess, NULL, 0, 
         (LPTHREAD_START_ROUTINE)(DWORD) pdwCodeRemote,
         pInjLibInfoRemote, 0, &dwThreadId);
      if (hThread == NULL)
	  {
		  //WriteLog("CreateRemoteProcess failed");
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
		  VirtualFreeEx(hProcess, pInjLibInfoRemote, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		  return false;
	  }
	  //WriteLog("CreateRemoteThread success, WaitForSingleObject...");
      WaitForSingleObject(hThread, INFINITE);

      if (hThread != NULL)
	  {
         GetExitCodeThread(hThread, (PDWORD) &hinstDllRemote);
         CloseHandle(hThread);
      }
      // Let the remote thread start executing the remote 
      // ThreadFunc function using our modified stack, which
      // now contains an initialized INJLIBINFO structure.
      if(pdwCodeRemote)
	  {
		  VirtualFreeEx(hProcess, pdwCodeRemote, 0, MEM_RELEASE);
	  }
      if(pInjLibInfoRemote)
	  {
		  VirtualFreeEx(hProcess, pInjLibInfoRemote, 0, MEM_RELEASE);
	  }
	//WriteLog("OK++++");
		CloseHandle(hProcess);
	return(hinstDllRemote != NULL);
}


