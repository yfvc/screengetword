#ifndef _FILTER_H_

#define FILTER_EXECUTE	1
#define FILTER_OPEN		2
#define FILTER_CREATE	3
#define FILTER_WRITE	4
#define FILTER_READ		5
#define FILTER_COPY		6
#define FILTER_MOVE		7
#define FILTER_CHDIR	8
#define FILTER_OPENDIR	9		// folder
#define FILTER_CREATEDIR	10
#define FILTER_MOVEDIR	11
#define FILTER_DELETE		12
#define FILTER_EXECRYPT	13
#define FILTER_HIDEPROCESS	14
#define FILTER_DLLCRYPT	14

typedef struct
{
	int type;
	char app[40];
	int f_permit;
	char password[21];
}FILTER_INFO;

class CFileFilter
{
public:
	CFileFilter();
	~CFileFilter();

	FILTER_INFO *m_pFilterInfo;
	int m_nFilterInfoCount;
	int FilterName(char *name, int type);
	int FilterExecute(char *cmd);
	int FilterExeCrypt(char *cmd, char *new_cmd);
	int FilterOpenFile(char *fname);
	int FilterCreateFile(char *fname);
	int FilterWriteFile(char *fname);
	int FilterReadFile(char *fname);
	int FilterCopyFile(char *fsrc, char *fdst);
	int FilterMoveFile(char *fname, char *fname2);
	int FilterChDir(char *dir);
	int FilterOpenDir(char *dir);
	int FilterCreateDir(char *dir);
	int FilterDeleteFile(char *fname);
};

#endif
