// ZQQClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CZQQClientDlg �Ի���
class CZQQClientDlg : public CDialog
{
// ����
public:
	CZQQClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ZQQCLIENT_DIALOG };

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
	afx_msg void OnLogin();
	CString m_strUserName;
	CString m_strPassword;
	CString m_strServerIP;
	unsigned int m_nServerPort;
	afx_msg void OnReset();

	sockaddr_in m_serverAddr;
	SOCKET m_sClient;
	CEdit m_editHistoryMsg;
	afx_msg void OnSendMessage();
	CString m_strMessage;
};
