// ProcProtectMFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CProcProtectMFCApp:
// �йش����ʵ�֣������ ProcProtectMFC.cpp
//

class CProcProtectMFCApp : public CWinApp
{
public:
	CProcProtectMFCApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CProcProtectMFCApp theApp;