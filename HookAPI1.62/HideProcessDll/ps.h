typedef unsigned long NTSTATUS;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef unsigned long DWORD;
typedef long LONG;
typedef __int64 LONGLONG;

typedef struct {
	USHORT Length;
	USHORT MaxLen;
	USHORT *Buffer;
} UNICODE_STRING;

typedef struct {
	ULONG NextEntryDelta;
	ULONG ThreadCount;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	ULONG BasePriority;
	ULONG ProcessId;
}PROCESS_INFO;

typedef DWORD (WINAPI *NTQUERYSYSTEMINFORMATION)(
			IN ULONG SysInfoClass,
			IN OUT PVOID SystemInformation,
			IN ULONG SystemInformationLength,
			OUT PULONG RetLen);
