///////////////////////////////////////////////////////
// Andriy Oriekhov. 2006. Toleron Sofware.
// www.toleron.com
///////////////////////////////////////////////////////


#ifndef _NTPROCDRV_H_
#define _NTPROCDRV_H_


#ifdef _DEBUG
#define DBGPRINT(Fmt)										\
	{														\
		DbgPrint Fmt;										\
	}
#else
#define DBGPRINT(Fmt)
#endif

#define FILE_DEVICE_UNKNOWN             0x00000022
#define IOCTL_UNKNOWN_BASE              FILE_DEVICE_UNKNOWN
#define IOCTL_NTPROCDRV_SET_ADDRNFO		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)


#define uszDriverName				L"\\Device\\NTProcDrv"
#define uszDeviceName				L"\\DosDevices\\NTProcDrv"

//NtCreateProcess
typedef NTSTATUS (* NTCREATEPROCESS)(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN BOOLEAN InheritObjectTable,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL);

NTCREATEPROCESS OldNtCreateProcess;

NTSTATUS NewNtCreateProcess(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN BOOLEAN InheritObjectTable,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL);

//NtCreateThread
typedef NTSTATUS (* NTCREATETHREAD)(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ProcessHandle,
    OUT PCLIENT_ID ClientId,
    IN PCONTEXT ThreadContext,
    IN PINITIAL_TEB InitialTeb,
    IN BOOLEAN CreateSuspended);

NTCREATETHREAD OldNtCreateThread;

NTSTATUS NewNtCreateThread(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ProcessHandle,
    OUT PCLIENT_ID ClientId,
    IN PCONTEXT ThreadContext,
    IN PINITIAL_TEB InitialTeb,
    IN BOOLEAN CreateSuspended);

//NtTerminateProcess
typedef NTSTATUS (*NTTERMINATEPROCESS)(
    IN HANDLE ProcessHandle OPTIONAL,
    IN NTSTATUS ExitStatus);

NTTERMINATEPROCESS OldNtTerminateProcess;

NTSTATUS NewNtTerminateProcess(
    IN HANDLE ProcessHandle OPTIONAL,
    IN NTSTATUS ExitStatus);

//NtCreateProcessEx
typedef NTSTATUS (*NTCREATEPROCESSEX)(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN BOOLEAN InheritObjectTable,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL,
    IN HANDLE Unknown );

NTCREATEPROCESSEX OldNtCreateProcessEx;

NTSTATUS NewNtCreateProcessEx(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN BOOLEAN InheritObjectTable,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL,
    IN HANDLE Unknown );


//
// Private storage 
//
typedef struct _AddrInfo
{
	BOOLEAN	isInitialized;
	ULONG	SSTIndexNtCreateProcess;
	ULONG	SSTIndexNtCreateThread;
	ULONG	SSTIndexNtTerminateProcess;
	ULONG	SSTIndexNtCreateProcessEx;
	PVOID	pfnLoadLibrary;
} CAddrInfo, *PAddrInfo;

typedef struct _DEVICE_EXTENSION 
{
    PDEVICE_OBJECT	DeviceObject;
    CAddrInfo		AddrInfo;
	BOOLEAN			isHookedState;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

NTSTATUS HookServices(PAddrInfo	pAddrInfo, BOOLEAN isHookedState);
void UnHookServices(PAddrInfo	pAddrInfo, BOOLEAN isHookedState);

void UnloadDriver(PDRIVER_OBJECT DriverObject);
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DispatchIoctl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);


extern PDEVICE_OBJECT g_pDeviceObject;

#endif _NTPROCDRV_H_