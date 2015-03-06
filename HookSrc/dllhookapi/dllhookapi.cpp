// dllhookapi.cpp : Defines the entry point for the DLL application.
//
///////////////////////////////////////////////////////
// Andriy Oriekhov. 2006. Toleron Sofware.
// www.toleron.com
///////////////////////////////////////////////////////

#include "stdafx.h"
#include "windows.h"



BOOL WINAPI DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL,"Before starting process.","Hello",0);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL,"Before ending.","Hello",0);
		break;
	}

	return TRUE;
}

