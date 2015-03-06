///////////////////////////////////////////////////////
// Andriy Oriekhov. 2006. Toleron Sofware.
// www.toleron.com
///////////////////////////////////////////////////////


#ifndef _UNDOCUMENTED_H_
#define _UNDOCUMENTED_H_

#pragma pack(push,1)
typedef struct _KSERVICE_TABLE_DESCRIPTOR {
    PULONG_PTR Base;
    PULONG Count;
    ULONG Limit;
    PUCHAR Number;
} KSERVICE_TABLE_DESCRIPTOR, *PKSERVICE_TABLE_DESCRIPTOR;

typedef struct _INITIAL_TEB {

  PVOID                StackBase;
  PVOID                StackLimit;
  PVOID                StackCommit;
  PVOID                StackCommitMax;
  PVOID                StackReserved;

} INITIAL_TEB, *PINITIAL_TEB;

#pragma pack(pop)

#define NUMBER_SERVICE_TABLES 4

__declspec(dllimport) KSERVICE_TABLE_DESCRIPTOR KeServiceDescriptorTable[NUMBER_SERVICE_TABLES];



//typedef EPROCESS *PEPROCESS;


__declspec(dllimport) NTSTATUS
ZwAllocateVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN OUT PSIZE_T RegionSize,
    IN ULONG AllocationType,
    IN ULONG Protect
    );


__declspec(dllimport) VOID
KeAttachProcess (
    IN void * Process); // Pcb

__declspec(dllimport) VOID
KeDetachProcess (
    VOID);

#endif