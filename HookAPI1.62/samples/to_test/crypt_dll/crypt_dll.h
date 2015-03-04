
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DES_DLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DES_DLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef DES_DLL_EXPORTS
#define DES_DLL_API __declspec(dllexport)
#else
#define DES_DLL_API __declspec(dllimport)
#endif

extern unsigned char des_key[8];

int encrypt_file(char *file_src, char *file_cry);
int decrypt_file(char *file_crypt, char *file_src);
