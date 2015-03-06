#ifndef _MONITORING_
#define _MONITORING_



#define NT4_DRIVERNAME "\\\\.\\NTProcDrv"
#define OTHER_DRIVERNAME "\\\\.\\Global\\NTProcDrv"
#define PROCDRVPROCESSEVENT "NTProcDrvProcessEvent"

typedef void (__stdcall *COnCreateProcess)(DWORD dwProcesID);
typedef void (__stdcall *COnTerminateProcess)(DWORD dwProcesID);

class CNtDriverController;
typedef struct _AddrInfo
{
	BOOLEAN	isInitialized;
	ULONG	SSTIndexNtCreateProcess;
	ULONG	SSTIndexNtCreateThread;
	ULONG	SSTIndexNtTerminateProcess;
	ULONG	SSTIndexNtCreateProcessEx;
	PVOID	pfnLoadLibrary;
} CAddrInfo, *PAddrInfo;

class CMonitoring
{
public:
    CMonitoring();
    ~CMonitoring();
    
    BOOL EndMonitoring(void);
	BOOL SetWindowsInfoToDriver(BOOL isStart);
	BOOL BeginMonitoring(void);


private:
    HANDLE m_hDriver;

	unsigned GetNtSSTIndex(LPCSTR szFuncName);

};
#endif