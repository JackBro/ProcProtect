#ifndef _MD5_H
#define _MD5_H

/***********************************************************************
* ��������:MD5File 
* ��������:�����ļ���MD5ֵ
* �����б�:
*		pStrFileName:�ļ���
*		pStrFileMD5:�õ���MD5
* ����ֵ:NTSTATUS
* ע:
*	pStrFileNameӦ��Ϊ������·������ʹ�÷������ӻ����豸��
*	���� "C:\\1.txt" Ӧ��Ϊ "\\??\\C:\\1.txt" ���� "\\Device\\HarddiskVolume1\\1.txt"
*	pStrFileMD5�ڴ����ⲿ�������
***********************************************************************/
NTSTATUS
MD5File(IN PUNICODE_STRING pStrFileName,OUT PUNICODE_STRING pStrFileMD5);

/***********************************************************************
* ��������:MD5String 
* ��������:�����ַ�����MD5ֵ
* �����б�:
*		pStr:�ַ���
*		pStrMD5:����õ���MD5
* ����ֵ:NTSTATUS
*	pStrFileMD5�ڴ����ⲿ�������
***********************************************************************/
NTSTATUS
MD5String(IN PUNICODE_STRING pStr,OUT PUNICODE_STRING pStrMD5);


#endif

