#include <ntifs.h>
#include "ProcList.h"

// ������Ҫ��������PID
LIST_ENTRY SafeProcHead;

/***********************************************************************
* ��������:InitSafeProcList 
* ��������:��ʼ��SafeProcList
* �����б�:
* ����ֵ:
***********************************************************************/
VOID 
InitSafeProcList()
{
	InitializeListHead(&SafeProcHead);
}

/***********************************************************************
* ��������:AddToSafeProcList 
* ��������:������ID��ӵ��Ϸ����̱���
* �����б�:
*		PID:����ID
* ����ֵ:NTSTATUS
***********************************************************************/
NTSTATUS
AddToSafeProcList(IN const HANDLE PID)
{
	PSAFEPROC pData;
	pData = (PSAFEPROC)ExAllocatePool(NonPagedPool,
		sizeof(SAFEPROC));
	if (pData == NULL)
	{
	//	KdPrint(("[AddToSafeProcList] ExAllocatePool Failed\n"));
		return STATUS_UNSUCCESSFUL;
	}
	pData->PID = PID;
	InsertTailList(&SafeProcHead,&pData->ListEntry);
//	KdPrint(("[AddToSafeProcList] Add %d\n",PID));
	return TRUE;
}

/***********************************************************************
* ��������:DelFromSafeProcList 
* ��������:������ID�ӺϷ����̱���ɾ��
* �����б�:
*		PID:����ID
* ����ֵ:
***********************************************************************/
VOID
DelFromSafeProcList(IN const HANDLE PID)
{
	PSAFEPROC pData;
	pData = (PSAFEPROC)SafeProcHead.Flink;
	while(pData != (PSAFEPROC)&SafeProcHead)
	{
		if (pData->PID == PID)
		{
			// ɾ�����PID
			pData->ListEntry.Blink->Flink = pData->ListEntry.Flink;
			pData->ListEntry.Flink->Blink = pData->ListEntry.Blink;
		//	KdPrint(("[DelFromSafeProcList] Del %d\n",PID));
			ExFreePool(pData);
			break;
		}
		pData = (PSAFEPROC)pData->ListEntry.Flink;
	}
	return;
}

/***********************************************************************
* ��������:IsSafeProc 
* ��������:��ѯ�Ƿ��ǺϷ�����
* �����б�:
*		PID:���̵�PID
* ����ֵ:BOOLEAN
***********************************************************************/
BOOLEAN
IsSafeProc(IN const HANDLE PID)
{
	PSAFEPROC pData;
	pData = (PSAFEPROC)SafeProcHead.Flink;
	while(pData != (PSAFEPROC)&SafeProcHead)
	{
		if (pData->PID == PID)
		{
			// ���PID�ǺϷ�����
		//	KdPrint(("[IsSafeProc] Get %d\n",PID));
			return TRUE;
		}
		pData = (PSAFEPROC)pData->ListEntry.Flink;
	}
	return FALSE;
}

/***********************************************************************
* ��������:ClearSafeProcList 
* ��������:
* �����б�:
*		PID:����ID
* ����ֵ:
***********************************************************************/
VOID
ReleaseSafeProcList()
{
	while (!IsListEmpty(&SafeProcHead))
	{
		PLIST_ENTRY pEntry = RemoveTailList(&SafeProcHead);
		PSAFEPROC pData = (PSAFEPROC)pEntry;
	//	KdPrint(("[ReleaseSafeProcList] Del %d\n",pData->PID));
		KdPrint(("����PID = %d��ȡ����������\n",pData->PID));
		ExFreePool(pData);
	}
	return;
}