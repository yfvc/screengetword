
#ifndef __mydll_h__
#define __mydll_h__

#define _WIN32_WINNT 0x0400

#include <stdio.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <windows.h>
#include <wininet.h>


typedef struct
{
	char *module_name;
	char *api_name;
	int param_count;
	char *my_api_name;
	int start_pos;
	char *friend_my_api_name;
}MYAPIINFO;


#endif
