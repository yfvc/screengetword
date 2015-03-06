///////////////////////////////////////////////////////
// Andriy Oriekhov. 2006. Toleron Sofware.
// www.toleron.com
///////////////////////////////////////////////////////

#include <ntddk.h>
#include "DrvUtils.h"
#include "Undocumented.h"

PCWatchedProcesses g_pWatchedPrecesses = NULL;
int isQueueInitialized = 0;
FAST_MUTEX g_QueueAccessFastMutex;



NTSTATUS InitializeMemoryPool(void)
{
	NTSTATUS ResultStatus = STATUS_SUCCESS;
	SIZE_T QueueSize = sizeof(CCWatchedProcesses) * MAX_QUEUE_NUMBER;

	g_pWatchedPrecesses = MmAllocateNonCachedMemory(QueueSize);
	if (g_pWatchedPrecesses == NULL)
	{
		ResultStatus = STATUS_UNSUCCESSFUL;
	}
	else
	{
		ExInitializeFastMutex(&g_QueueAccessFastMutex);

		RtlZeroMemory(g_pWatchedPrecesses, QueueSize);
		isQueueInitialized = 1;
	}
	return ResultStatus;
}

void UnitializeMemoryPool(void)
{
	SIZE_T QueueSize = sizeof(CCWatchedProcesses) * MAX_QUEUE_NUMBER;

	if (isQueueInitialized && g_pWatchedPrecesses!=NULL)
	{
		ExAcquireFastMutex(&g_QueueAccessFastMutex);
		MmFreeNonCachedMemory(g_pWatchedPrecesses, QueueSize);
		g_pWatchedPrecesses = NULL;
		isQueueInitialized = 1;
		ExReleaseFastMutex(&g_QueueAccessFastMutex);
	}
}

PCWatchedProcesses GetNextAvailableAddr()
{
	ULONG isFound;
	ULONG i;
	PCWatchedProcesses pReturnAddr;

	isFound = 0;
	pReturnAddr = g_pWatchedPrecesses;

	if (g_pWatchedPrecesses==NULL)
		return NULL;

	ExAcquireFastMutex(&g_QueueAccessFastMutex);
	for (i=0; i<MAX_QUEUE_NUMBER; i++)
	{
		if (pReturnAddr->ProcessHandle == NULL)
		{
			isFound = 1;
			break;
		}
		pReturnAddr++;
	}
	if (!isFound)
		pReturnAddr = NULL;

	ExReleaseFastMutex(&g_QueueAccessFastMutex);

	return pReturnAddr;
}

NTSTATUS SetNewProcessInfo(HANDLE ProcessHandle)
{
	NTSTATUS ResultStatus;
	PCWatchedProcesses pNewAddr;

	if (g_pWatchedPrecesses==NULL)
		return STATUS_UNSUCCESSFUL;

	
	ResultStatus = STATUS_SUCCESS;


	pNewAddr = GetNextAvailableAddr();

	if (pNewAddr==NULL)
	{
		ResultStatus = STATUS_UNSUCCESSFUL;
	}
	else
	{
		ExAcquireFastMutex(&g_QueueAccessFastMutex);
		pNewAddr->ProcessHandle = ProcessHandle;
		ExReleaseFastMutex(&g_QueueAccessFastMutex);
	}

	return ResultStatus;

}



NTSTATUS RemoveProcessFromQueue(HANDLE ProcessHandle)
{
	NTSTATUS ResultStatus;
	BOOLEAN isFound;
	PCWatchedProcesses pCurrentAddr;
	ULONG i;

	if (g_pWatchedPrecesses==NULL)
		return STATUS_UNSUCCESSFUL;

	pCurrentAddr = g_pWatchedPrecesses;

	ResultStatus = STATUS_SUCCESS;

	isFound = FALSE;
	ExAcquireFastMutex(&g_QueueAccessFastMutex);
	for (i=0; i<MAX_QUEUE_NUMBER; i++)
	{
		if (pCurrentAddr->ProcessHandle==ProcessHandle)
		{
			RtlZeroMemory(pCurrentAddr,sizeof(CCWatchedProcesses));
			isFound = TRUE;
			break;
		}
		pCurrentAddr++;
	}

	ExReleaseFastMutex(&g_QueueAccessFastMutex);
	if (!isFound)
	{
		ResultStatus = STATUS_UNSUCCESSFUL;
	}

	return ResultStatus;
}


BOOLEAN FindLockProcessInQueue(HANDLE ProcessHandle)
{
	BOOLEAN isProcessFound = FALSE;
	PCWatchedProcesses pCurrentAddr;

	if (g_pWatchedPrecesses!=NULL)
	{
		ULONG i;

		pCurrentAddr = g_pWatchedPrecesses;

		ExAcquireFastMutex(&g_QueueAccessFastMutex);

		for (i=0; i<MAX_QUEUE_NUMBER; i++)
		{
			if (pCurrentAddr->ProcessHandle==ProcessHandle)
			{
				isProcessFound = TRUE;
				break;
			}
			pCurrentAddr++;
		}

		if (!isProcessFound)
			ExReleaseFastMutex(&g_QueueAccessFastMutex);
	}
	return isProcessFound;
}

NTSTATUS UnlockProcessQueue(void)
{
	ExReleaseFastMutex(&g_QueueAccessFastMutex);
	return STATUS_SUCCESS;
}