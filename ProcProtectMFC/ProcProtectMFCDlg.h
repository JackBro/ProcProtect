// ProcProtectMFCDlg.h : ͷ�ļ�
//

#pragma once


// CProcProtectMFCDlg �Ի���
class CProcProtectMFCDlg : public CDialog
{
// ����
public:
	CProcProtectMFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PROCPROTECTMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInstallhook();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedUninstallhook();
	afx_msg void OnBnClickedSaveexeinfo();
};
