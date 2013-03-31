// ProcProtectMFCDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProcProtectMFC.h"
#include "ProcProtectMFCDlg.h"
#include <winioctl.h>
#include "..\ProcProtect\IOCTL.h"

#define SymLinkName "\\\\.\\ProcProtect"
#define ExeName "notepad.exe"
HANDLE hDevice = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProcProtectMFCDlg �Ի���




CProcProtectMFCDlg::CProcProtectMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcProtectMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcProtectMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProcProtectMFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_InstallHook, &CProcProtectMFCDlg::OnBnClickedInstallhook)
	ON_BN_CLICKED(IDC_Start, &CProcProtectMFCDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_UnInstallHook, &CProcProtectMFCDlg::OnBnClickedUninstallhook)
	ON_BN_CLICKED(IDC_SaveExeInfo, &CProcProtectMFCDlg::OnBnClickedSaveexeinfo)
END_MESSAGE_MAP()


// CProcProtectMFCDlg ��Ϣ�������

BOOL CProcProtectMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// �����������
	hDevice = CreateFile(SymLinkName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL );

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		MessageBox("CreateFile Failed!");
		return FALSE;
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CProcProtectMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CProcProtectMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CProcProtectMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CProcProtectMFCDlg::OnBnClickedInstallhook()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet;
	DWORD dwOutput;
	bRet = DeviceIoControl(hDevice, IOCTL_Install_SSDTHook,
		NULL,0,
		NULL, 0,
		&dwOutput, NULL);
	if (bRet == FALSE)
	{
		MessageBox("IOCTL_Install_SSDTHook Failed");
		return;
	}
/*
	int iPID = GetCurrentProcessId();
	bRet = DeviceIoControl(hDevice, IOCTL_AddPortectProc,
		&iPID,sizeof(iPID),
		NULL, 0,
		&dwOutput, NULL);
	if (bRet == FALSE)
	{
		MessageBox("IOCTL_AddPortectProc Failed");
		return;
	}
*/
}

void CProcProtectMFCDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	/*
	// ����notepad
	STARTUPINFO si; //һЩ�ر���������
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi; //�ر��������ý���
	BOOL bRet = CreateProcess(NULL,ExeName,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	*/
	
	BOOL bRet;
	DWORD dwOutput;
	int iPID = 2100;
	bRet = DeviceIoControl(hDevice, IOCTL_AddPortectProc,
		&iPID,sizeof(iPID),
		NULL, 0,
		&dwOutput, NULL);
	if (bRet == FALSE)
	{
		MessageBox("IOCTL_AddPortectProc Failed");
		return;
	}
}

void CProcProtectMFCDlg::OnBnClickedUninstallhook()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwOutput;
	BOOL bRet = DeviceIoControl(hDevice, IOCTL_UnInstall_SSDTHook,
		NULL, 0, 
		NULL, 0,
		&dwOutput, NULL);
	if (bRet == FALSE)
	{
		MessageBox("IOCTL_UnInstall_SSDTHook Failed");
		return;
	}
}


void CProcProtectMFCDlg::OnBnClickedSaveexeinfo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwOutput;
	WCHAR wcExe[0x200];
//	wcscpy(wcExe,L"C:\\windows\\notepad.exe");
//	wcscpy(wcExe,L"C:\\Program Files\\Microsoft Visual Studio 9.0\\Common7\\IDE\\devenv.exe");
	wcscpy(wcExe,L"c:\\windows\\system32\\GDI32.DLL");
	
	BOOL bRet = DeviceIoControl(hDevice, IOCTL_SetSafeExe,
		wcExe, 0x200, 
		NULL, 0,
		&dwOutput, NULL);
	if (bRet == FALSE)
	{
		MessageBox("IOCTL_UnInstall_SSDTHook Failed");
		return;
	}
}
