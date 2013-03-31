#include <ntifs.h>
#include "BaseFun.h"
#include "SSDTHook.h"
#include "MD5.h"
#include "ProcList.h"

// ����ԭʼ��SSDT��SSDTShadow������
ULONG ulOldSSDTTable[MAX_SSDT_ITEM_NUM];
ULONG ulOldSSDTShadowTable[MAX_SSDT_ITEM_NUM];

// ���SSDT��SSDTShadow���ַ
extern "C" PSSDT KeServiceDescriptorTable;
PSSDT KeServiceDescriptorTableShadow;

// SSDT��SSDTShadow���ڴ�MDL
PMDL pSSDTMdl = NULL;
PMDL pSSDTShadowMdl = NULL;

// MDLӳ�������SSDT��SSDTShadow���ַ
SSDT SSDTMdlEntry;
SSDT SSDTShadowMdlEntry;

// ������Ҫ��������PID
extern LIST_ENTRY SafeProcHead;

extern "C" NTSTATUS ZwQueryInformationProcess(
	__in       HANDLE ProcessHandle,
	__in       PROCESSINFOCLASS ProcessInformationClass,
	__out      PVOID ProcessInformation,
	__in       ULONG ProcessInformationLength,
	__out_opt  PULONG ReturnLength
	);


/***********************************************************************
* ��������:InstallSDDTHook 
* ��������:��װSSDTHook
* �����б�:
*		��
* ����ֵ:״ֵ̬
* ע:
***********************************************************************/
NTSTATUS
InstallSDDTHook()
{
//	KdPrint(("InstallSDDTHook\n"));
	// ʹSSDT���д
	// ����MDL
	pSSDTMdl = MmCreateMdl(NULL,
		KeServiceDescriptorTable->pvSSDTBase,
		KeServiceDescriptorTable->ulNumberOfServices * 4);
	if(!pSSDTMdl)
	{
		KdPrint(("[InstallSDDTHook] MmCreateMdl pSSDTMdl Failed\n"));
		return STATUS_UNSUCCESSFUL;
	}
	// �����Ƿ�ҳ�ڴ��
	MmBuildMdlForNonPagedPool(pSSDTMdl);
	// ����Ϊ��д
	pSSDTMdl->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
	// �����ɵ�SSDT�ṹ��
	memcpy(&SSDTMdlEntry,
		KeServiceDescriptorTable,
		sizeof(SSDT));
	// ��������ڴ棬�����µ�SSDT�ṹ���ַ
	SSDTMdlEntry.pvSSDTBase = (ULONG*)MmMapLockedPages(pSSDTMdl,
		KernelMode);


	// ʹSSDTShadow���д
	// ����SSDTShadow��ַ
	KeServiceDescriptorTableShadow = FindSSDTShadow();
	if (!KeServiceDescriptorTableShadow)
	{
		KdPrint(("[InstallSDDTHook] FindSSDTShadow Failed\n"));
		return STATUS_UNSUCCESSFUL;
	}
	// ����MDL
	pSSDTShadowMdl = MmCreateMdl(NULL,
		KeServiceDescriptorTableShadow->pvSSDTBase,
		KeServiceDescriptorTableShadow->ulNumberOfServices*4);
	if(!pSSDTShadowMdl)
	{
		KdPrint(("[InstallSDDTHook] MmCreateMdl pSSDTShadowMdl Failed\n"));
		return STATUS_UNSUCCESSFUL;
	}
	// �����Ƿ�ҳ�ڴ��
	MmBuildMdlForNonPagedPool(pSSDTShadowMdl);
	// ����Ϊ��д
	pSSDTShadowMdl->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
	// �����ɵ�SSDT�ṹ��
	memcpy(&SSDTShadowMdlEntry,
		KeServiceDescriptorTableShadow,
		sizeof(SSDT));
	// ��������ڴ棬�����µ�SSDT�ṹ���ַ
	SSDTShadowMdlEntry.pvSSDTBase = (ULONG*)MmMapLockedPages(pSSDTShadowMdl,
		KernelMode);

	// ����ԭʼ��
	SaveSSDTTable(&SSDTMdlEntry,ulOldSSDTTable);
	SaveSSDTTable(&SSDTShadowMdlEntry,ulOldSSDTShadowTable);

	// ���SSDT����
	/*
	InstallSSDTHookByIndex(&SSDTMdlEntry,
		nt_NtOpenProcess, 
		(ULONG)HookNtOpenProcess);
	*/
	/*
	InstallSSDTHookByIndex(&SSDTMdlEntry,
		nt_NtCreateThread, 
		(ULONG)HookNtCreateThread);
	*/
	/*
	InstallSSDTHookByIndex(&SSDTMdlEntry,
		nt_NtCreateProcessEx, 
		(ULONG)HookNtCreateProcessEx);
	*/
	/*
	InstallSSDTHookByIndex(&SSDTMdlEntry,
		nt_NtCreateSection, 
		(ULONG)HookNtCreateSection);
	*/
	/*
	InstallSSDTHookByIndex(&SSDTMdlEntry,
		nt_NtMapViewOfSection, 
		(ULONG)HookNtMapViewOfSection);
	*/
	// ���SSDTShadow����
	
	InstallSSDTHookByIndex(&SSDTShadowMdlEntry,
		win32k_NtUserSetWindowsHookEx,
		(ULONG)HookNtUserSetWindowsHookEx);
	/*
	InstallSSDTHookByIndex(&SSDTShadowMdlEntry,
		win32k_NtUserMessageCall,
		(ULONG)HookNtUserMessageCall);
	*/
	return STATUS_SUCCESS;
}

/***********************************************************************
* ��������:UnInstallSDDTHook 
* ��������:ж��SSDTHook
* �����б�:
*		��
* ����ֵ:VOID
* ע:
***********************************************************************/
VOID
UnInstallSDDTHook()
{
//	KdPrint(("UnInstallSDDTHook\n"));

	// �ͷŽ��̱����б�
	// �ָ�ԭʼ��SSDT��SSDTShadow������
	LoadSSDTTable(&SSDTMdlEntry,ulOldSSDTTable);
	LoadSSDTTable(&SSDTShadowMdlEntry,ulOldSSDTShadowTable);

	// ����MDL��
	MmUnmapLockedPages(SSDTMdlEntry.pvSSDTBase,pSSDTMdl);
	MmUnmapLockedPages(SSDTShadowMdlEntry.pvSSDTBase,pSSDTShadowMdl);

	// �ͷ�MDL
	IoFreeMdl(pSSDTMdl);
	IoFreeMdl(pSSDTShadowMdl);
}

/***********************************************************************
* ��������:FindSSDTShadow 
* ��������:ɨ�ڴ����KeServiceDescriptorTableShadow��ַ����ΧΪ+-SSDTSHADOW_MAX_SEARCH_LENGTH * sizeof(SSDT)
* �����б�:
*		��
* ����ֵ:KeServiceDescriptorTableShadow��ַ
* ע:
*	http://bbs.pediy.com/showthread.php?p=898895
***********************************************************************/
PSSDT
FindSSDTShadow()
{
	PSSDT KeServiceDescriptorTableShadow = KeServiceDescriptorTable - SSDTSHADOW_MAX_SEARCH_LENGTH * sizeof(SSDT);

	// ɨ�ڴ�
	while(KeServiceDescriptorTableShadow <= KeServiceDescriptorTable + SSDTSHADOW_MAX_SEARCH_LENGTH * sizeof(SSDT))
	{
		if (RtlCompareMemory(KeServiceDescriptorTable,
			KeServiceDescriptorTableShadow,
			sizeof(SSDT)) == sizeof(SSDT))
		{
			// �ҵ���KeServiceDescriptorTable
			KeServiceDescriptorTableShadow++;
			break;
		}
		KeServiceDescriptorTableShadow++;
	}

	// �ж��Ƿ��ҵ�
	if (KeServiceDescriptorTableShadow > KeServiceDescriptorTable + SSDTSHADOW_MAX_SEARCH_LENGTH * sizeof(SSDT))
	{
		// δ�ҵ�
		return NULL;
	}

	return KeServiceDescriptorTableShadow;
}

/***********************************************************************
* ��������:SaveSSDTTable 
* ��������:����SSDT�еĺ�����ַ
* �����б�:
*		pSSDT:��ȡλ��
*		SSDTTable:���SSDT������
* ����ֵ:��
***********************************************************************/
VOID
SaveSSDTTable(IN PSSDT pSSDT,
			  OUT ULONG SSDTTable[MAX_SSDT_ITEM_NUM])
{
	PULONG pAddr = pSSDT->pvSSDTBase;
	for (ULONG i = 0; i < pSSDT->ulNumberOfServices;i++,pAddr++)
		SSDTTable[i] = *pAddr;
}

/***********************************************************************
* ��������:LoadOldSSDTTable 
* ��������:�ָ�SSDT�еĺ�����ַ
* �����б�:
*		pSSDT:�ָ���λ��
*		SSDTTable:���SSDT������
* ����ֵ:��
***********************************************************************/
VOID
LoadSSDTTable(IN PSSDT pSSDT,
			  IN ULONG SSDTTable[MAX_SSDT_ITEM_NUM])
{
	PULONG pAddr = pSSDT->pvSSDTBase;
	for (ULONG i = 0; i < pSSDT->ulNumberOfServices;i++,pAddr++)
		*pAddr = SSDTTable[i];
}

/***********************************************************************
* ��������:InstallSSDTHookByIndex
* ��������:ͨ����������SSDT HOOK
* �����б�:
*		pSSDT:���ַ
*		ulServerNum:�����
*		pNewService:hook������ַ
* ����ֵ:
***********************************************************************/
VOID
InstallSSDTHookByIndex(PSSDT pSSDT,
					   ULONG ulServerNum,
					   ULONG pNewService)
{
	pSSDT->pvSSDTBase[ulServerNum] = pNewService;
	return;
}

/***********************************************************************
* ��������:UnInstallSSDTHookByIndex
* ��������:ͨ�������ж��SSDT HOOK
* �����б�:
*		pSSDT:���ַ
*		ulServerNum:�����
*		ulSSDTTable:ԭʼ��
* ����ֵ:
***********************************************************************/
VOID
UnInstallSSDTHookByIndex(PSSDT pSSDT,
						 ULONG ulServerNum,
						 ULONG ulSSDTTable[MAX_SSDT_ITEM_NUM])
{
	pSSDT->pvSSDTBase[ulServerNum] = ulSSDTTable[ulServerNum];
	return;
}


/***********************************************************************
* ��������:HookNtOpenProcess
* ��������:NtOpenProcess��HOOK
* �����б�:
*		...
* ����ֵ:��
* ע:
*	
***********************************************************************/
// ����ԭʼ����ָ��
typedef NTSTATUS (*NtOpenProcessType) (__out PHANDLE,
									   __in ACCESS_MASK,
									   __in POBJECT_ATTRIBUTES,
									   __in_opt PCLIENT_ID);
NtOpenProcessType pOldNtOpenProcess = NULL;

NTSTATUS
HookNtOpenProcess (
				   __out PHANDLE  ProcessHandle,
				   __in ACCESS_MASK  DesiredAccess,
				   __in POBJECT_ATTRIBUTES  ObjectAttributes,
				   __in_opt PCLIENT_ID  ClientId
				   )
{
	if (IsSafeProc(ClientId->UniqueProcess)
		&& !IsSafeProc(PsGetCurrentProcessId())
		)
	{
		KdPrint(("[HookNtOpenProcess] CurrentPID = %d ,ClientId->UniqueProcess = %d\n",
			PsGetCurrentProcessId(),ClientId->UniqueProcess));
		return STATUS_ACCESS_DENIED;
	}

	if (pOldNtOpenProcess == NULL)
	{
		// ��ȡԭʼSSDTָ��ĺ���
		pOldNtOpenProcess = (NtOpenProcessType)ulOldSSDTTable[nt_NtOpenProcess];
	}
	// ����ԭ����
	return pOldNtOpenProcess(ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ClientId);
}


/***********************************************************************
* ��������:HookNtCreateThread
* ��������:NtCreateThread��HOOK
* �����б�:
*		...
* ����ֵ:��
* ע:
*	
***********************************************************************/
typedef NTSTATUS (*NtCreateThreadType)(
									   __out PHANDLE ThreadHandle,
									   __in ACCESS_MASK DesiredAccess,
									   __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
									   __in HANDLE ProcessHandle,
									   __out PCLIENT_ID ClientId,
									   __in PCONTEXT ThreadContext,
									   __in PVOID InitialTeb,
									   __in BOOLEAN CreateSuspended
									   );
NtCreateThreadType pOldNtCreateThread = NULL;

NTSTATUS HookNtCreateThread(
							__out PHANDLE ThreadHandle,
							__in ACCESS_MASK DesiredAccess,
							__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
							__in HANDLE ProcessHandle,
							__out PCLIENT_ID ClientId,
							__in PCONTEXT ThreadContext,
							__in PVOID InitialTeb,
							__in BOOLEAN CreateSuspended
							)
{

	NTSTATUS status;
	// ��ѯ������Ϣ
	PROCESS_BASIC_INFORMATION pbi;
	status = ZwQueryInformationProcess(ProcessHandle, ProcessBasicInformation, 
		(PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);

	if (!NT_SUCCESS(status))
	{
		KdPrint(("[HookNtCreateThread] ZwQueryInformationProcess Failed\n"));
		return pOldNtCreateThread(
			ThreadHandle,
			DesiredAccess,
			ObjectAttributes,
			ProcessHandle,
			ClientId,
			ThreadContext,
			InitialTeb,
			CreateSuspended
			);
	}

	// �õ�PID
	ULONG CurrentPID = (ULONG)PsGetCurrentProcessId();
	ULONG TargetPID = pbi.UniqueProcessId;

	if ( IsSafeProc((HANDLE)TargetPID) 
		&& !IsSafeProc((HANDLE)CurrentPID) 
		)
	{
		// ��Ҫ�������Ľ���
		if (CurrentPID != TargetPID)
		{
			// �����Լ������̣߳���Զ�߳�
			KdPrint(("[HookNtCreateThread] CurrentPID = %d,TargetPID = %d Stoped",
				CurrentPID,TargetPID));
			return STATUS_ACCESS_DENIED;
		}
	}

	if (pOldNtCreateThread == NULL)
	{
		pOldNtCreateThread = (NtCreateThreadType)ulOldSSDTTable[nt_NtCreateThread];
	}

	return pOldNtCreateThread(
		ThreadHandle,
		DesiredAccess,
		ObjectAttributes,
		ProcessHandle,
		ClientId,
		ThreadContext,
		InitialTeb,
		CreateSuspended
		);
}



/***********************************************************************
* ��������:HookNtUserSetWindowsHookEx
* ��������:NtUserSetWindowsHookEx��HOOK
* �����б�:
*		...
* ����ֵ:��
* ע:
*	
***********************************************************************/
typedef HHOOK (*NtUserSetWindowsHookExType)(
	HINSTANCE Mod, 
	PUNICODE_STRING UnsafeModuleName, 
	DWORD ThreadId, 
	int HookId, 
	PVOID HookProc, 
	BOOL Ansi);

NtUserSetWindowsHookExType pOldNtUserSetWindowsHookEx = NULL;

HHOOK HookNtUserSetWindowsHookEx(
								 HINSTANCE Mod, 
								 PUNICODE_STRING UnsafeModuleName, 
								 DWORD ThreadId, 
								 int HookId, 
								 PVOID HookProc,
								 BOOL Ansi)
{
	// ��ѯ�߳���������
	// http://www.osronline.com/showThread.cfm?link=3172
	NTSTATUS status = STATUS_SUCCESS;
	PETHREAD pEThread = NULL;
	status = PsLookupThreadByThreadId((HANDLE)ThreadId,&pEThread);
	if(!NT_SUCCESS(status))
	{
		return NULL;
	}
	ULONG TargetPID = *(ULONG *)((UCHAR *)pEThread + 0x1ec);
	ObDereferenceObject(pEThread);

//	KdPrint(("[HookNtUserSetWindowsHookEx] UnsafeModuleName = %wZ,TargetThreadId = 0x%x,CurrentPID = %d,TargetPID = %d\n",
//		UnsafeModuleName,ThreadId,PsGetCurrentProcessId(),TargetPID));
	KdPrint(("��⵽SetWindowsHookEx�������� PID = %d\n",TargetPID));
	ULONG CurrentPID = (ULONG)PsGetCurrentProcessId();
	if ( IsSafeProc((HANDLE)TargetPID) && TargetPID != CurrentPID)
	{
		// ���ҹ��Ľ������Ǳ����Ľ��̣����Ҳ����Լ��ҹ�
		return NULL;
	}

	if (pOldNtUserSetWindowsHookEx == NULL)
	{
		pOldNtUserSetWindowsHookEx = (NtUserSetWindowsHookExType)ulOldSSDTShadowTable[win32k_NtUserSetWindowsHookEx];
	}
	return pOldNtUserSetWindowsHookEx(
		Mod, 
		UnsafeModuleName, 
		ThreadId, 
		HookId, 
		HookProc, 
		Ansi);
}


/***********************************************************************
* ��������:NtUserSetWindowsHookExType
* ��������:NtUserMessageCall��HOOK
* �����б�:
*		...
* ����ֵ:��
* ע:
*	
***********************************************************************/
typedef BOOL (*NtUserMessageCallType)(
									  HWND hWnd,
									  UINT Msg,
									  WPARAM wParam,
									  LPARAM lParam,
									  ULONG_PTR ResultInfo,
									  DWORD dwType,
									  BOOL Ansi);
NtUserMessageCallType pOldNtUserMessageCall = NULL;

typedef DWORD (*NtUserQueryWindowType)(
									   HWND hWnd,
									   DWORD Index	// QUERY_WINDOW_
									   );
NtUserQueryWindowType pOldNtUserQueryWindow = NULL;

BOOL HookNtUserMessageCall(
						   HWND hWnd,
						   UINT Msg,
						   WPARAM wParam,
						   LPARAM lParam,
						   ULONG_PTR ResultInfo,
						   DWORD dwType,
						   BOOL Ansi)
{
	if (pOldNtUserQueryWindow == NULL)
	{
		pOldNtUserQueryWindow = (NtUserQueryWindowType)ulOldSSDTShadowTable[win32k_NtUserQueryWindow];
	}
	HANDLE TargetPID = (HANDLE)pOldNtUserQueryWindow(hWnd,QUERY_WINDOW_UNIQUE_PROCESS_ID);

	if (PsGetCurrentProcessId() != TargetPID && IsSafeProc(TargetPID))
	{
		KdPrint(("[HookNtUserMessageCall] CurrentPID = %d, TargetPID = %d\n",
			PsGetCurrentProcessId(),TargetPID));
		return FALSE;
	}
	if (pOldNtUserMessageCall == NULL)
	{
		pOldNtUserMessageCall = (NtUserMessageCallType)ulOldSSDTShadowTable[win32k_NtUserMessageCall];
	}
	return pOldNtUserMessageCall(
		hWnd,
		Msg,
		wParam,
		lParam,
		ResultInfo,
		dwType,
		Ansi);
}


/***********************************************************************
* ��������:HookNtCreateProcessExType
* ��������:NtCreateProcessExType��HOOK
* �����б�:
*		...
* ����ֵ:��
* ע:
*	
***********************************************************************/
typedef NTSTATUS (*NtCreateProcessExType)(
	__out PHANDLE ProcessHandle,
	__in ACCESS_MASK DesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
	__in HANDLE ParentProcess,
	__in ULONG Flags,
	__in_opt HANDLE SectionHandle,
	__in_opt HANDLE DebugPort,
	__in_opt HANDLE ExceptionPort,
	__in ULONG JobMemberLevel
	);

NtCreateProcessExType pOldNtCreateProcessEx = NULL;

NTSTATUS HookNtCreateProcessEx(
							   __out PHANDLE ProcessHandle,
							   __in ACCESS_MASK DesiredAccess,
							   __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
							   __in HANDLE ParentProcess,
							   __in ULONG Flags,
							   __in_opt HANDLE SectionHandle,
							   __in_opt HANDLE DebugPort,
							   __in_opt HANDLE ExceptionPort,
							   __in ULONG JobMemberLevel
							   )
{
	NTSTATUS retStatus;
	if (pOldNtCreateProcessEx == NULL)
	{
		pOldNtCreateProcessEx = (NtCreateProcessExType)ulOldSSDTTable[nt_NtCreateProcessEx];
	}
	retStatus = pOldNtCreateProcessEx(
		ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ParentProcess,
		Flags,
		SectionHandle,
		DebugPort,
		ExceptionPort,
		JobMemberLevel
		);

	// ����ļ���
	// SectionHandle->pSection->pSegment->pControlArea->pFileObject
	// ���嶨��ο�wrk
	// struct _SECTION
	PVOID pSection = NULL;
	NTSTATUS status = ObReferenceObjectByHandle(SectionHandle,
		NULL,
		NULL,
		KernelMode,
		&pSection,
		NULL
		);
	if (!NT_SUCCESS(status))
	{
		return retStatus;
	}
	PVOID pSegment = (PVOID)*(ULONG*)((char*)pSection + 20);
	PVOID pControlArea = (PVOID)*(ULONG*)pSegment;
	PVOID pFileObject = (PVOID)*(ULONG*)((char*)pControlArea + 36);
	UNICODE_STRING usFullName;
	WCHAR wFullNameBuf[0x200];
	RtlInitEmptyUnicodeString(&usFullName,
		wFullNameBuf,
		sizeof(wFullNameBuf));
	GetFullPathNameFromFileObject((PFILE_OBJECT)pFileObject,
		&usFullName);
	ObDereferenceObject(pSection);

	// �������ж���α������Ľ����Ƿ������ǵ�������EXE
	UNICODE_STRING usCorrectMD5;
	RtlInitUnicodeString(&usCorrectMD5,MAINEXE_MD5);
	UNICODE_STRING usTargetMD5;
	WCHAR wTargetMD5Buf[0x200];
	RtlInitEmptyUnicodeString(&usTargetMD5,wTargetMD5Buf,sizeof(wTargetMD5Buf));
	status = MD5File(&usFullName,&usTargetMD5);

	if (RtlCompareUnicodeString(&usCorrectMD5,&usTargetMD5,TRUE) == 0)
	{
		// �½����������ǵ�������EXE
		// �����*ProcessHandle�ǽ��̾��������PID
		// ��ѯ������Ϣ
		PROCESS_BASIC_INFORMATION pbi;
		ZwQueryInformationProcess(*ProcessHandle, ProcessBasicInformation, 
			(PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
		KdPrint(("[HookNtCreateProcessEx] MainEXE Started!PID = %d\n",pbi.UniqueProcessId));
		AddToSafeProcList((HANDLE)pbi.UniqueProcessId);
		return retStatus;
	}

	if (IsSafeProc(PsGetCurrentProcessId()))
	{
		// �������ǺϷ�����
		// �����*ProcessHandle�ǽ��̾��������PID
		// ��ѯ������Ϣ
		PROCESS_BASIC_INFORMATION pbi;
		ZwQueryInformationProcess(*ProcessHandle, ProcessBasicInformation, 
			(PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
		KdPrint(("[HookNtCreateProcessEx]  MainExe Create New Safe Proc,PID = %d\n",pbi.UniqueProcessId));
		AddToSafeProcList((HANDLE)pbi.UniqueProcessId);
	}
	return retStatus;
}

typedef NTSTATUS (*NtCreateSectionType)(
										__out PHANDLE SectionHandle,
										__in ACCESS_MASK DesiredAccess,
										__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
										__in_opt PLARGE_INTEGER MaximumSize,
										__in ULONG SectionPageProtection,
										__in ULONG AllocationAttributes,
										__in_opt HANDLE FileHandle
										);
NtCreateSectionType pOldNtCreateSection = NULL;
NTSTATUS HookNtCreateSection(
							 __out PHANDLE SectionHandle,
							 __in ACCESS_MASK DesiredAccess,
							 __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
							 __in_opt PLARGE_INTEGER MaximumSize,
							 __in ULONG SectionPageProtection,
							 __in ULONG AllocationAttributes,
							 __in_opt HANDLE FileHandle
							 )
{
	if (pOldNtCreateSection == NULL)
	{
		pOldNtCreateSection = (NtCreateSectionType)ulOldSSDTTable[nt_NtCreateSection];
	}

	if (IsSafeProc(PsGetCurrentProcessId()))
	{
		if (ObjectAttributes != NULL)
		{
			KdPrint(("[HookNtCreateSection] PID = %d ,ObjName = %wZ\n",PsGetCurrentProcessId(),ObjectAttributes->ObjectName));
		}
		else if(FileHandle != NULL)
		{
			PVOID pFileObj = NULL;
			NTSTATUS status = ObReferenceObjectByHandle(FileHandle,
				NULL,
				*IoFileObjectType,
				KernelMode,
				&pFileObj,
				NULL
				);
			if (!NT_SUCCESS(status))
			{
				goto END;
			}
			UNICODE_STRING usFullName;
			WCHAR wFullNameBuf[0x200];
			RtlInitEmptyUnicodeString(&usFullName,wFullNameBuf,sizeof(wFullNameBuf));
			GetFullPathNameFromFileObject((PFILE_OBJECT)pFileObj,
				&usFullName);
			ObDereferenceObject(pFileObj);
			KdPrint(("[HookNtCreateSection] PID = %d ,ObjName = %wZ\n",PsGetCurrentProcessId(),&usFullName));
		}
	}
END:
	NTSTATUS retStatus;
	retStatus = pOldNtCreateSection(
		SectionHandle,
		DesiredAccess,
		ObjectAttributes,
		MaximumSize,
		SectionPageProtection,
		AllocationAttributes,
		FileHandle
		);

	return retStatus;
}

typedef NTSTATUS (*NtMapViewOfSectionType)(
				   __in HANDLE SectionHandle,
				   __in HANDLE ProcessHandle,
				   __inout PVOID *BaseAddress,
				   __in ULONG_PTR ZeroBits,
				   __in SIZE_T CommitSize,
				   __inout_opt PLARGE_INTEGER SectionOffset,
				   __inout PSIZE_T ViewSize,
				   __in SECTION_INHERIT InheritDisposition,
				   __in ULONG AllocationType,
				   __in WIN32_PROTECTION_MASK Win32Protect
				   );
NtMapViewOfSectionType pOldNtMapViewOfSection = NULL;

NTSTATUS HookNtMapViewOfSection(
								__in HANDLE SectionHandle,
								__in HANDLE ProcessHandle,
								__inout PVOID *BaseAddress,
								__in ULONG_PTR ZeroBits,
								__in SIZE_T CommitSize,
								__inout_opt PLARGE_INTEGER SectionOffset,
								__inout PSIZE_T ViewSize,
								__in SECTION_INHERIT InheritDisposition,
								__in ULONG AllocationType,
								__in WIN32_PROTECTION_MASK Win32Protect
								)
{
	// ��ѯ������Ϣ
	NTSTATUS status;
	PROCESS_BASIC_INFORMATION pbi;
	status = ZwQueryInformationProcess(ProcessHandle, ProcessBasicInformation, 
		(PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
//	ASSERT(NT_SUCCESS(status));

	// �õ�PID
	HANDLE CurrentPID = PsGetCurrentProcessId();
	HANDLE TargetPID = (HANDLE)pbi.UniqueProcessId;

	if (IsSafeProc(TargetPID))
	{
		// ����ļ���
		// SectionHandle->pSection->pSegment->pControlArea->pFileObject
		// ���嶨��ο�wrk
		// struct _SECTION
		PVOID pSection = NULL;
		status = ObReferenceObjectByHandle(SectionHandle,
			NULL,
			NULL,
			KernelMode,
			&pSection,
			NULL
			);
		ASSERT(NT_SUCCESS(status));

		PVOID pSegment = (PVOID)*(ULONG*)((char*)pSection + 20);
		if(pSegment == NULL)
		{
			KdPrint(("pSegment == NULL"));
			ObDereferenceObject(pSection);
			goto END;
		}
		PVOID pControlArea = (PVOID)*(ULONG*)pSegment;
		if(pControlArea == NULL)
		{
			KdPrint(("pControlArea == NULL"));
			ObDereferenceObject(pSection);
			goto END;
		}
		PVOID pFileObject = (PVOID)*(ULONG*)((char*)pControlArea + 36);
		if(pFileObject == NULL)
		{
			KdPrint(("pFileObject == NULL,Win32Protect = 0x%x\n",Win32Protect));
			ObDereferenceObject(pSection);
			goto END;
		}
		UNICODE_STRING usFullName;
		WCHAR wFullNameBuf[0x200];
		RtlInitEmptyUnicodeString(&usFullName,
			wFullNameBuf,
			sizeof(wFullNameBuf));
		GetFullPathNameFromFileObject((PFILE_OBJECT)pFileObject,
			&usFullName);
		KdPrint(("[HookNtMapViewOfSection] CurrentPID = %d,TargetPID = %d,SectionName = %wZ,Win32Protect = 0x%x\n",
			CurrentPID,TargetPID,&usFullName,Win32Protect));
		ObDereferenceObject(pSection);
	}
END:
	if (pOldNtMapViewOfSection == NULL)
	{
		pOldNtMapViewOfSection = (NtMapViewOfSectionType)ulOldSSDTTable[nt_NtMapViewOfSection];
	}
	return pOldNtMapViewOfSection(
		SectionHandle,
		ProcessHandle,
		BaseAddress,
		ZeroBits,
		CommitSize,
		SectionOffset,
		ViewSize,
		InheritDisposition,
		AllocationType,
		Win32Protect
		);
}