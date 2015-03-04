#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include <winsock.h>

//#include "Resource.h"
//#include "Util.h"
//#include "Psapi.h"


//#include <shlwapi.h>

extern HINSTANCE g_hInstance;

char *strnstr_nbuf ( char *buf, char *text, int bufsz )
{
	char *t = (char *)malloc (bufsz+1);
	char *result;
	strncpy (t, buf, bufsz);
	*(t+bufsz)=0;
	result = strstr (t, text);
	free(t);
	return result;
}

char *strnstr_ntext ( char *buf, char *text, int textsz )
{
	char *t = (char *)malloc (textsz+1);
	char *result;
	strncpy (t, text, textsz);
	*(t+textsz)=0;
	result = strstr (buf, t);
	free(t);
	return result;
}


void StrChrSubst ( char *s, char c1, char c2)
	{
	while (*s!=0)
		{
		if (*s==c1)	*s = c2;
		s++;
		}
	}

void GetFileName(char *fname)
{
	char temp[200];

	GetModuleFileName(NULL, temp, sizeof(temp));
	int i =strlen(temp);
	while(i >0 && temp[i-1] !='\\' && temp[i-1] !=':') i--;
	strcpy(fname, &temp[i]);
	strupr(fname);
}

void WriteLog(char *fmt,...) 
{
	FILE *fp;
	va_list args;
	char modname[200];
	
	if((fp =fopen("c:\\APIHook.log", "a")) !=NULL)
	{		
		va_start(args,fmt);
		
		GetModuleFileName(NULL, modname, sizeof(modname));
		fprintf(fp, "HookFunc.dll:%s:", modname);
		vfprintf(fp, fmt, args);
		fprintf(fp, "\n");
		fclose(fp);
		
		va_end(args);
	}
}

void WriteLog2( char *buf, int bufsz ) 
{
	FILE *fp;

	if((fp =fopen("c:\\APIHook2.log", "a")) !=NULL)
	{		
		for (int n=0; n<bufsz; n++)
		{
			fputc ( *(buf+n), fp );
		}
		fprintf(fp, "\0");
		fclose(fp);		
	}
}


char * GetErrString(char *str, DWORD errcode)
{
	LPVOID lpbuf;
	
	if(FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpbuf,
		0,
		NULL
		))
	{
		lstrcpy(str, (char *)lpbuf);
		LocalFree(lpbuf);
	}
	
	return str;
}


void DisplayError()
{
	LPVOID lpMsgBuf;
	FormatMessage	( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
									FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
									MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
									(LPTSTR) &lpMsgBuf, 0, NULL );
	
	// Display the string.
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );

}

void XorString ( char *s )
{
	while (*s!=0)
	{
		*s = 255-*s;
		s++;
	}
}

void SplitBuf ( char *buf, char *name, char *value )
{
	while ( *buf != '|' )
	{
		*(name++) = *(buf++);
	}
	*name = 0;
	buf++; 
	while ( *buf != 0 )
	{
		*(value++) = *(buf++);
	}
	*value = 0;
}

/*
void b64_encode (unsigned char *str, int len, unsigned char *out)
{
    int i;
    unsigned char *cur = out;
    static char table[64] = {
        'A','B','C','D','E','F','G','H',
        'I','J','K','L','M','N','O','P',
        'Q','R','S','T','U','V','W','X',
        'Y','Z','a','b','c','d','e','f',
        'g','h','i','j','k','l','m','n',
        'o','p','q','r','s','t','u','v',
        'w','x','y','z','0','1','2','3',
        '4','5','6','7','8','9','+','/'
    };

    for (i = 0; i < len; i += 3)
    {
        *cur++ = table[str[0] >> 2];
        *cur++ = table[((str[0] & 3) << 4) + (str[1] >> 4)];
        *cur++ = table[((str[1] & 0xf) << 2) + (str[2] >> 6)];
        *cur++ = table[str[2] & 0x3f];
        str += 3;
    }

    if (i == len + 1)
        *(cur - 1) = '=';
    else if (i == len + 2)
        *(cur - 1) = *(cur - 2) = '=';
    *cur = '\0';
}*/

int StrChrCount ( char *s, char c )
	{
	int n = 0;
	while (*s != 0)
		{
		n += (*(s++) == c);
		}
	return n;
	}
/*
BOOL myStrCmp ( LPCTSTR s1, LPCTSTR s2, int exact_match = 0 )
	{
	if (exact_match)
		return (StrCmpIA ( s1, s2) == 0); 
	else
	  return (StrStrIA ( s1, s2) != 0);
	}

*/