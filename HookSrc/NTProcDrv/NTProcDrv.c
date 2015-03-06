//---------------------------------------------------------------------------
//
// NTProcDrv.c
//
// SUBSYSTEM: 
//				System monitor
// MODULE:    
//				Driver for intercepting process creation routines
//
// DESCRIPTION:
//              This code is based on the James Finnegan’s sample 
//              (MSJ January 1999) and Ivo Ivanov sample (http://www.codeproject.com/system/hooksys.asp).
//
// Andriy Oriekhov, 2006
//                                                                         
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Includes
//  
//---------------------------------------------------------------------------
#include <ntddk.h>
#include "Undocumented.h"
#include "NTProcDrv.h"
#include "DrvUtils.h"



PDEVICE_OBJECT g_pDeviceObject = NULL;

//
// The main entry point of the driver module
//
NTSTATUS __stdcall DriverEntry(
	IN PDRIVER_OBJECT DriverObject, 
	IN PUNICODE_STRING RegistryPath
	)
{
    NTSTATUS        ntStatus;
    UNICODE_STRING  uszDriverString;
    UNICODE_STRING  uszDeviceString;

    PDEVICE_OBJECT    pDeviceObject;
    PDEVICE_EXTENSION extension;
    
	DBGPRINT(("In DriverEntry\n"));
	
	// Point uszDriverString at the driver name
    RtlInitUnicodeString(&uszDriverString, uszDriverName);

    // Create and initialize device object
    ntStatus = IoCreateDevice(
		DriverObject,
        sizeof(DEVICE_EXTENSION),
        &uszDriverString,
        FILE_DEVICE_UNKNOWN,
        0,
        FALSE,
        &pDeviceObject
		);
    if(ntStatus != STATUS_SUCCESS)
        return ntStatus;
    
	// Assign extension variable
    extension = pDeviceObject->DeviceExtension;
	RtlZeroMemory(&extension->AddrInfo,sizeof(CAddrInfo));
    
	// Point uszDeviceString at the device name
    RtlInitUnicodeString(&uszDeviceString, uszDeviceName);
    
	// Create symbolic link to the user-visible name
    ntStatus = IoCreateSymbolicLink(&uszDeviceString, &uszDriverString);

    if(ntStatus != STATUS_SUCCESS)
    {
        // Delete device object if not successful
        IoDeleteDevice(pDeviceObject);
        return ntStatus;
    }

    // Assign global pointer to the device object for use by the callback functions
    g_pDeviceObject = pDeviceObject;

    // Load structure to point to IRP handlers
    DriverObject->DriverUnload                         = UnloadDriver;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;


	if (InitializeMemoryPool()==STATUS_SUCCESS)
	{
		DBGPRINT(("Memory for Pool successfully initialized\n"));
	}
	else
	{
		DBGPRINT(("Memory Pool Initialization failed\n"));
	}

    // Return success
    return ntStatus;
}

//
// Create and close routine
//
NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information=0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}


NTSTATUS DispatchIoctl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    NTSTATUS              ntStatus = STATUS_UNSUCCESSFUL;
    PIO_STACK_LOCATION    irpStack  = IoGetCurrentIrpStackLocation(Irp);
    PDEVICE_EXTENSION     extension = DeviceObject->DeviceExtension;
    PAddrInfo	pAddrInfo;
	
	//
    // These IOCTL handlers get the current data out of the device
    // extension structure.  
	//
    switch(irpStack->Parameters.DeviceIoControl.IoControlCode)
    {
        case IOCTL_NTPROCDRV_SET_ADDRNFO:
			if (irpStack->Parameters.DeviceIoControl.InputBufferLength>= sizeof(CAddrInfo))
			{
				DBGPRINT(("receiving IOCTL_NTPROCDRV_SET_ADDRNFO ok!\n"));

				pAddrInfo = Irp->AssociatedIrp.SystemBuffer;

				if (pAddrInfo->isInitialized)
				{

					extension->AddrInfo.SSTIndexNtCreateProcess = pAddrInfo->SSTIndexNtCreateProcess;
					extension->AddrInfo.SSTIndexNtCreateThread = pAddrInfo->SSTIndexNtCreateThread;
					extension->AddrInfo.SSTIndexNtTerminateProcess = pAddrInfo->SSTIndexNtTerminateProcess;
					extension->AddrInfo.pfnLoadLibrary = pAddrInfo->pfnLoadLibrary;
					extension->AddrInfo.SSTIndexNtCreateProcessEx = pAddrInfo->SSTIndexNtCreateProcessEx;
					extension->AddrInfo.isInitialized = TRUE;

					DBGPRINT(("Trying to hook srvices\n"));
					HookServices(&extension->AddrInfo, extension->isHookedState);
					extension->isHookedState = TRUE;
				}
				else
				{
					DBGPRINT(("Trying to unhook srvices\n"));
					UnHookServices(&extension->AddrInfo,extension->isHookedState);
					extension->isHookedState = FALSE;
					RtlZeroMemory(&extension->AddrInfo,sizeof(CAddrInfo));
				}

				ntStatus = STATUS_SUCCESS;

			}
            break;
        default:
            break;
    }

    Irp->IoStatus.Status = ntStatus;
   
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return ntStatus;
}


//
// Driver unload routine
//
void UnloadDriver(IN PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING  uszDeviceString;
	PDEVICE_EXTENSION extension = DriverObject->DeviceObject->DeviceExtension;

	UnHookServices(&extension->AddrInfo,extension->isHookedState);

	UnitializeMemoryPool();


    IoDeleteDevice(DriverObject->DeviceObject);

    RtlInitUnicodeString(&uszDeviceString, uszDeviceName);
    IoDeleteSymbolicLink(&uszDeviceString);
}


NTSTATUS NewNtCreateProcess(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN BOOLEAN InheritObjectTable,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL
    )
{
	NTSTATUS ResultStatus;

	DBGPRINT(("In NtCreateProcess\n"));

	
	ResultStatus = OldNtCreateProcess(
	    ProcessHandle,
	    DesiredAccess,
		ObjectAttributes,
		ParentProcess,
		InheritObjectTable,
		SectionHandle,
		DebugPort ,
		ExceptionPort);

	

	if (ResultStatus==STATUS_SUCCESS)
	{

		DBGPRINT(("Kernel successfully create NtCreateProcess.\n----->ProcessHandle = 0x%x\n",*ProcessHandle));
	    
		if (SetNewProcessInfo(*ProcessHandle)==STATUS_SUCCESS)
		{
			DBGPRINT(("Successfully added process handle to Queue\n"));
		}
		else
		{
			DBGPRINT(("Unable to add process handle to Queue\n"));
		}
	}

	return ResultStatus;

}

NTSTATUS NewNtCreateProcessEx(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN BOOLEAN InheritObjectTable,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL,
    IN HANDLE Unknown )
{
	NTSTATUS ResultStatus;

	DBGPRINT(("In NtCreateProcessEx\n"));
	
	ResultStatus = OldNtCreateProcessEx(
	    ProcessHandle,
	    DesiredAccess,
		ObjectAttributes,
		ParentProcess,
		InheritObjectTable,
		SectionHandle,
		DebugPort ,
		ExceptionPort,
		Unknown);

	

	if (ResultStatus==STATUS_SUCCESS)
	{

		DBGPRINT(("Kernel successfully create NtCreateProcessEx.\n----->ProcessHandle = 0x%x\n",*ProcessHandle));
	    
		if (SetNewProcessInfo(*ProcessHandle)==STATUS_SUCCESS)
		{
			DBGPRINT(("Successfully added process handle to Queue\n"));
		}
		else
		{
			DBGPRINT(("Unable to add process handle to Queue\n"));
		}
	}

	return ResultStatus;
}


NTSTATUS NewNtCreateThread(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ProcessHandle,
    OUT PCLIENT_ID ClientId,
    IN PCONTEXT ThreadContext,
    IN PINITIAL_TEB InitialTeb,
    IN BOOLEAN CreateSuspended)
{
	NTSTATUS ResultStatus;

#pragma message("must verify user mode parameters before using")

	if (
		ThreadContext && 
		CreateSuspended && 
		ProcessHandle && 
		ProcessHandle!=(void *)0xffffffff && 
		FindLockProcessInQueue(ProcessHandle)
		)
	{

		//kernel creates thread
		//trying to get PEPROCESS

		NTSTATUS ObjectStatus;
		PEPROCESS pProcess;
		POBJECT_TYPE PsProcessType;

		DBGPRINT(("Kernel trying to CreateThread(from User Mode) for ProcessHandle = 0x%X\n",ProcessHandle));

		PsProcessType = NULL;
		ObjectStatus = ObReferenceObjectByHandle(
			ProcessHandle,
			PROCESS_ALL_ACCESS,
			PsProcessType,
			UserMode,
			(PVOID *)&pProcess,
			NULL);
		if (ObjectStatus!=STATUS_SUCCESS)
		{
			DBGPRINT(("Fail to RefernceObject. Stopping Injecting Technique.\n"));
		}
		else
		{
			NTSTATUS AllocStatus;
			UCHAR * BaseAddr;
			SIZE_T RegionSize;
			ULONG Win32StartAddr;
			ULONG LoadLibraryAddr;

			RegionSize = 100;
			BaseAddr = NULL;
			PsProcessType = NULL;

			LoadLibraryAddr = (ULONG)((PDEVICE_EXTENSION)g_pDeviceObject->DeviceExtension)->AddrInfo.pfnLoadLibrary;
			DBGPRINT(("Address of LoadLibraryW = 0x%08X\n",LoadLibraryAddr));


			if (ThreadContext)
			{
				//remembering value of ThreadContext->Eax
				//it contains process staring address in user mode

				Win32StartAddr = ThreadContext->Eax;
				
				if (!Win32StartAddr)
				{
					DBGPRINT(("--------------Win32StartAddr not set--------------------\n"));
				}
				else
				{

					//"jumping" to context of new process
					KeDetachProcess();
					KeAttachProcess((void *)pProcess);

					//allocating memory with PAGE_EXECUTE_READWRITE access rights

					AllocStatus = ZwAllocateVirtualMemory(
						NtCurrentProcess(),
						&BaseAddr,
						0,
						&RegionSize,
						MEM_COMMIT	,
						PAGE_EXECUTE_READWRITE);
					if (AllocStatus!=STATUS_SUCCESS)
					{
						DBGPRINT(("---------------Fail to Allocate memory! AllocStatus = 0x%x\n",AllocStatus));
					}
					else
					{
						ULONG Offset = 0;
						ULONG OffsetStrAddr = 0;

						DBGPRINT(("---------------Memory allocation OK! BaseAddr = 0x%x\n",BaseAddr));

						BaseAddr[Offset] = 0x68; //push
						++Offset;
						OffsetStrAddr = Offset;
						Offset += 4; //reserving 4 byte for string address. will be filled late

						BaseAddr[Offset] = 0xbb; //mov ebx,LoadLibraryAddr
						++Offset;

						*(ULONG *)(BaseAddr+Offset) = LoadLibraryAddr; 
						Offset += 4;

						BaseAddr[Offset] = 0xff; //call ebx
						++Offset;
						BaseAddr[Offset] = 0xd3;
						++Offset;

						BaseAddr[Offset] = 0xb8;						 //mov eax,Win32StartAddr
						++Offset;
						*(ULONG *)(BaseAddr+Offset) = Win32StartAddr; 
						Offset += 4;

						BaseAddr[Offset] = 0x50; //push eax
						++Offset;

						BaseAddr[Offset] = 0xc3; //ret
						++Offset;

						*(ULONG *)(BaseAddr+OffsetStrAddr) = (ULONG)(BaseAddr + Offset); // parameter for LoadLibraryW

						//forming string with DLL name - tunew20.dll

						BaseAddr[Offset] = 't';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 'u';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 'n';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 'e';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 'w';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = '2';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = '0';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = '.';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 'd';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 'l';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 'l';
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;
						BaseAddr[Offset] = 0x0;
						++Offset;


						DBGPRINT(("---------------Old EAX Value = 0x%08x\n",Win32StartAddr));
						Win32StartAddr = (ULONG)BaseAddr;
						


					}
					//"jumping" back to previos contex.

					KeDetachProcess();

					//setting process start address to allocated memory
					ThreadContext->Eax = Win32StartAddr;
				}
			
			}

			ObDereferenceObject(pProcess);
			
		}

		UnlockProcessQueue();

		RemoveProcessFromQueue(ProcessHandle);
	
	}

	ResultStatus = OldNtCreateThread(
		ThreadHandle,
		DesiredAccess,
		ObjectAttributes,
		ProcessHandle,
		ClientId,
		ThreadContext,
		InitialTeb,
		CreateSuspended);

	if (ResultStatus == STATUS_SUCCESS && CreateSuspended)
	{
		DBGPRINT(("Kernel sucessfully create new thread.\n----->ThreadHandle = 0x%xh for ProcessHandle = 0x%x\n",*ThreadHandle,ProcessHandle));
	}

	return ResultStatus;
}



NTSTATUS NewNtTerminateProcess(
    IN HANDLE ProcessHandle OPTIONAL,
    IN NTSTATUS ExitStatus)
{
	NTSTATUS ResultStatus;

#pragma message("must verify user mode parameters before use")
	
	DBGPRINT(("Called NtTerminateProcess\n"));

	if(ProcessHandle && ProcessHandle!=(void *)0xffffffff)
	{
		RemoveProcessFromQueue(ProcessHandle);
	}

	ResultStatus = OldNtTerminateProcess(ProcessHandle,ExitStatus);
	if (ResultStatus == STATUS_SUCCESS)
	{
		DBGPRINT(("Kernel sucessfully terminate Process.\n----->ProcessHandle = 0x%xh\n",ProcessHandle));
	}


	return ResultStatus;
}


NTSTATUS HookServices(PAddrInfo	pAddrInfo, BOOLEAN isHookedState)
{

	if (pAddrInfo && pAddrInfo->isInitialized)
	{
		BOOLEAN isNeedHookEx = FALSE;
		if (pAddrInfo->SSTIndexNtCreateProcessEx)
		{
			isNeedHookEx = TRUE;
		}

		DBGPRINT(("Setting hook on SSTIndexNtCreateProcess = 0x%08X\n",pAddrInfo->SSTIndexNtCreateProcess));
		DBGPRINT(("Setting hook on SSTIndexNtCreateThread = 0x%08X\n",pAddrInfo->SSTIndexNtCreateThread));
		DBGPRINT(("Setting hook on SSTIndexNtTerminateProcess = 0x%08X\n",pAddrInfo->SSTIndexNtTerminateProcess));
		DBGPRINT(("Setting hook on pfnLoadLibrary = 0x%08X\n",pAddrInfo->pfnLoadLibrary));
		if (isNeedHookEx)
			DBGPRINT(("Setting hook on SSTIndexNtCreateProcessEx = 0x%08X\n",pAddrInfo->SSTIndexNtCreateProcessEx));

	
		OldNtCreateProcess = (NTCREATEPROCESS)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateProcess]);
		OldNtCreateThread = (NTCREATETHREAD)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateThread]);
		OldNtTerminateProcess = (NTTERMINATEPROCESS)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtTerminateProcess]);
		if (isNeedHookEx)
			OldNtCreateProcessEx = (NTCREATEPROCESSEX)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateProcessEx]);

		_asm cli

		(NTCREATEPROCESS)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateProcess]) = NewNtCreateProcess;
		(NTCREATETHREAD)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateThread]) = NewNtCreateThread;
		(NTTERMINATEPROCESS)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtTerminateProcess]) = NewNtTerminateProcess;

		if (isNeedHookEx)
			(NTCREATEPROCESSEX)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateProcessEx]) = NewNtCreateProcessEx;

		_asm sti
	}

	return STATUS_SUCCESS;

}

void UnHookServices(PAddrInfo pAddrInfo, BOOLEAN isHookedState)
{

	if (pAddrInfo && pAddrInfo->isInitialized && isHookedState)
	{
		DBGPRINT(("Unhook service functions\n"));
		DBGPRINT(("SSTIndexNtCreateProcess = 0x%08X\n",pAddrInfo->SSTIndexNtCreateProcess));
		DBGPRINT(("SSTIndexNtCreateThread = 0x%08X\n",pAddrInfo->SSTIndexNtCreateThread));
		DBGPRINT(("SSTIndexNtTerminateProcess = 0x%08X\n",pAddrInfo->SSTIndexNtTerminateProcess));
		DBGPRINT(("pfnLoadLibrary = 0x%08X\n",pAddrInfo->pfnLoadLibrary));

		_asm cli

		(NTCREATEPROCESS)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateProcess])=OldNtCreateProcess;
		(NTCREATETHREAD)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateThread])=OldNtCreateThread;
		(NTTERMINATEPROCESS)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtTerminateProcess])=OldNtTerminateProcess;
		if (pAddrInfo->SSTIndexNtCreateProcessEx)
			(NTCREATEPROCESSEX)(KeServiceDescriptorTable[0].Base[pAddrInfo->SSTIndexNtCreateProcessEx])=OldNtCreateProcessEx;

		_asm sti
	}

	return;

}







