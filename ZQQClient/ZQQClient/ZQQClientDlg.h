// ZQQClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CZQQClientDlg 对话框
class CZQQClientDlg : public CDialog
{
// 构造
public:
	CZQQClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ZQQCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
