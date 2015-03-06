///////////////////////////////////////////////////////
// Andriy Oriekhov. 2006. Toleron Sofware.
// www.toleron.com
///////////////////////////////////////////////////////


#ifndef _DRVUTILS_H_
#define _DRVUTILS_H_

#pragma pack(push,1)


typedef struct _CWatchedProcesses
{
	HANDLE ProcessHandle;
} CCWatchedProcesses, * PCWatchedProcesses;

#pragma pack(pop)

#define MAX_QUEUE_NUMBER			1000


extern PCWatchedProcesses g_pWatchedPrecesses;

extern int isQueueInitialized;

extern FAST_MUTEX g_QueueAccessFastMutex;

NTSTATUS InitializeMemoryPool(void);
void UnitializeMemoryPool(void);
NTSTATUS SetNewProcessInfo(HANDLE ProcessHandle);
NTSTATUS RemoveProcessFromQueue(HANDLE ProcessHandle);
BOOLEAN FindLockProcessInQueue(HANDLE ProcessHandle);
NTSTATUS UnlockProcessQueue(void);

#endif _DRVUTILS_H_