#include <ntifs.h>
#include "BaseFun.h"


/***********************************************************************
* ��������:GetFullPathNameFromFileObject 
* ��������:ͨ��FILE_OBJECT����ļ�ȫ·����
* �����б�:
*		FileObj:�ļ�����
*		pStrFullName:·����
* ����ֵ:��
* ע:pStrFullName���ڴ�ռ�Ӧ�ں���������
***********************************************************************/
VOID 
GetFullPathNameFromFileObject(
							  IN PFILE_OBJECT FileObj,
							  OUT PUNICODE_STRING pStrFullName
							  )
{
	NTSTATUS status;
	CHAR nibuf[0x200];	//����һ���������ڴ漴��
	POBJECT_NAME_INFORMATION nameInfo = (POBJECT_NAME_INFORMATION)nibuf;
	ULONG retLength;
	ASSERT(FileObj->DeviceObject != NULL);
	status = ObQueryNameString(FileObj->DeviceObject,
		nameInfo,
		sizeof(nibuf),
		&retLength);
	pStrFullName->Length = 0;
	if (NT_SUCCESS(status))
	{
		RtlCopyUnicodeString(pStrFullName, &nameInfo->Name);
	}
	RtlAppendUnicodeStringToString(pStrFullName, &FileObj->FileName); 
}