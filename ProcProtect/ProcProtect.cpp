#include <ntifs.h>
#include "IOCTL.h"
#include "BaseFun.h"
#include "ProcProtect.h"
#include "SSDTHook.h"
#include "ProcList.h"
#include "MD5.h"
#include "ExeInfo.h"

// ����������
#define DeviceName L"\\Device\\ProcProtect"
#define SymLinkName L"\\??\\ProcProtect"

// ������Ҫ��������PID
extern LIST_ENTRY SafeProcHead;

/***********************************************************************
* ��������:DriverEntry
* ��������:������ں���
* �����б�:
*		pDriverObject:��������ָ��
*		pRegistryPath:ע���·��
* ����ֵ:NTSTATUS
* ע:
***********************************************************************/
extern "C" NTSTATUS DriverEntry (IN PDRIVER_OBJECT pDriverObject,
								 IN PUNICODE_STRING pRegistryPath)
{
//	KdPrint(("Enter ProcProtectDriverEntry\n"));
	NTSTATUS status = STATUS_SUCCESS;
/*
#ifdef DBG
	__asm int 3
#endif
*/
	// ע�������������ú������
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriverObject->MajorFunction[i] = ProcProtectDispatchRoutin;
	pDriverObject->DriverUnload = ProcProtectUnload;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcProtectDeviceIOControl;

	//�����豸
	PDEVICE_OBJECT pDevObj;
	UNICODE_STRING usDevName;
	RtlInitUnicodeString(&usDevName,DeviceName);
	status = IoCreateDevice(pDriverObject,
		0 ,
		&usDevName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj );
	if (!NT_SUCCESS(status))
		return status;

	// ���ö�дģʽΪ�������豸
	pDevObj->Flags |= DO_BUFFERED_IO;

	//������������
	UNICODE_STRING usSymLinkName;
	RtlInitUnicodeString(&usSymLinkName,SymLinkName);
	status = IoCreateSymbolicLink(&usSymLinkName,
		&usDevName);
	if (!NT_SUCCESS(status)) 
	{
		IoDeleteDevice(pDevObj);
		return status;
	}
	
	return status;
}

/***********************************************************************
* ��������:ProcMonDispatchRoutin
* ��������:����������ǲ����
* �����б�:
*		pDevObj:�豸����ָ��
*		pIrp:Irpָ��
* ����ֵ:NTSTATUS
* ע:
***********************************************************************/
NTSTATUS ProcProtectDispatchRoutin(IN PDEVICE_OBJECT pDevObj,
							   IN PIRP pIrp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	NTSTATUS status = STATUS_SUCCESS;

	// ���IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );

	return status;
}

/***********************************************************************
* ��������:ProcMonDeviceIOControl
* ��������:DeviceIOControl��ǲ����
* �����б�:
*		pDevObj:�豸����ָ��
*		pIrp:Irpָ��
* ����ֵ:NTSTATUS
* ע:
***********************************************************************/
NTSTATUS ProcProtectDeviceIOControl(IN PDEVICE_OBJECT pDevObj,
								IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG info = 0;

	//�õ���ǰ��ջ
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	//�õ�IOCTL��
	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;
/*
#ifdef DBG
	__asm int 3
#endif
*/
	HANDLE PID;
	switch (code)
	{
	case IOCTL_Install_SSDTHook:
		status = InstallSDDTHook();
		InitSafeProcList();
		break;
	case IOCTL_UnInstall_SSDTHook:
		UnInstallSDDTHook();
		ReleaseSafeProcList();
		break;
	case IOCTL_AddPortectProc:
		PID = *(HANDLE*)pIrp->AssociatedIrp.SystemBuffer;
		AddToSafeProcList(PID);
		KdPrint(("����PID = %d������Ϊ��������\n",PID));
		break;
	case IOCTL_DelPortectProc:
		PID = *(HANDLE*)pIrp->AssociatedIrp.SystemBuffer;
		DelFromSafeProcList(PID);
	//	KdPrint(("����PID = %d��ȡ����������\n",PID));
		break;
	case IOCTL_SetSafeExe:
		WCHAR wcExePath[0x200];
		memset(wcExePath,0,0x200);
		memcpy(wcExePath,L"\\??\\",8);
		memcpy(wcExePath+4,pIrp->AssociatedIrp.SystemBuffer,0x190);
		UNICODE_STRING usExePath;
		RtlInitUnicodeString(&usExePath,wcExePath);
		SetSafeExe(&usExePath);
		break;
	default:
		status = STATUS_INVALID_VARIANT;
	}

	// ���IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = info;	// bytes xfered
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );
	return status;
}

/***********************************************************************
* ��������:ProcMonUnload
* ��������:����ж�غ���
* �����б�:
*		pDriverObject:�豸��������
* ����ֵ:VOID
* ע:
***********************************************************************/
VOID ProcProtectUnload (IN PDRIVER_OBJECT pDriverObject)
{
	
	UNICODE_STRING usSymLinkName;
	RtlInitUnicodeString(&usSymLinkName,SymLinkName);
	IoDeleteSymbolicLink(&usSymLinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);
//	KdPrint(("ProcMonUnload!\n"));
}
