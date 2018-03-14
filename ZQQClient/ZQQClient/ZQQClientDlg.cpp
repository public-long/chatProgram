// ZQQClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ZQQClient.h"
#include "ZQQClientDlg.h"
#include "Protocol.h"

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI RecvThread(LPVOID lpParam)
{
	CZQQClientDlg* pDlg = (CZQQClientDlg*)lpParam;
	while(true)
	{
		char szRecvBuff[2048];
		/*int fromLen = sizeof(sockaddr);
		recvfrom(pDlg->m_sClient, szRecvBuff, sizeof(szRecvBuff), 
			0, (sockaddr*)&pDlg->m_serverAddr, &fromLen);*/

		recv(pDlg->m_sClient, szRecvBuff, sizeof(szRecvBuff),0);

		int pktType = ((Header*)szRecvBuff)->type;
		if(pktType == PUBLIC_CHAT_PKT)
		{
			PublicChatMsg* pkt = (PublicChatMsg*)szRecvBuff;
			char szOutText[1024];
			strcpy(szOutText, pkt->from);
			strcat(szOutText, ":");
			strcat(szOutText, pkt->message);
			strcat(szOutText, "\r\n");
			pDlg->m_editHistoryMsg.SetSel(0,0);
			pDlg->m_editHistoryMsg.ReplaceSel(szOutText);	
		}
	}
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	/*DDX_Text(pDX, IDC_SERVER_IP, m_strServerIP);
	DDX_Text(pDX, IDC_SERVER_PORT, m_nServerPort);
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);*/
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CZQQClientDlg 对话框




CZQQClientDlg::CZQQClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZQQClientDlg::IDD, pParent)
	, m_strUserName(_T("xxx"))
	, m_strPassword(_T("1234"))
	, m_strServerIP(_T("127.0.0.1"))
	, m_nServerPort(8888)
	, m_strMessage(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZQQClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTORY_MESSAGE, m_editHistoryMsg);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	DDX_Text(pDX, IDC_USER_NAME, m_strUserName);
}

BEGIN_MESSAGE_MAP(CZQQClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CZQQClientDlg::OnLogin)
	ON_BN_CLICKED(IDC_BTN_RESET, &CZQQClientDlg::OnReset)
	ON_BN_CLICKED(IDC_BTN_SEND, &CZQQClientDlg::OnSendMessage)
END_MESSAGE_MAP()


// CZQQClientDlg 消息处理程序

BOOL CZQQClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CZQQClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CZQQClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CZQQClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CZQQClientDlg::OnLogin()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	LoginPkt loginPkt;
	loginPkt.header.type = LOGIN_PKT;
	strcpy(loginPkt.userName, (LPCTSTR)m_strUserName);
	strcpy(loginPkt.password, (LPCTSTR)m_strPassword);

	//m_sClient = socket(AF_INET, SOCK_DGRAM, 0);
	m_sClient = socket(AF_INET, SOCK_STREAM, 0);
	memset(&m_serverAddr, 0, sizeof(m_serverAddr));
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.s_addr = inet_addr((LPCTSTR)m_strServerIP);
	m_serverAddr.sin_port = htons(m_nServerPort);

	connect(m_sClient, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
	//sendto(m_sClient, (const char*)&loginPkt, sizeof(loginPkt), 0,
	//	(const sockaddr*)&m_serverAddr, sizeof(sockaddr));
	send(m_sClient, (const char*)&loginPkt, sizeof(loginPkt), 0);

	char buff[2048];
	int addrLen = sizeof(sockaddr);
	//recvfrom(m_sClient, buff, 2048, 0, (sockaddr*)&m_serverAddr, &addrLen);
	recv(m_sClient, buff, 2048, 0);

	LoginRelpyPkt* replyPkt = (LoginRelpyPkt*)buff;
	if(LOGIN_SUCCESS == replyPkt->retCode)
	{
		AfxMessageBox("登录成功");
		DWORD dwThreadID;
		HANDLE hRecvThread = CreateThread(NULL, 0, 
			RecvThread, (LPVOID)this, 0, &dwThreadID);
		CloseHandle(hRecvThread);
	}
	else
	{
		AfxMessageBox("登录失败");
	}



}

void CZQQClientDlg::OnReset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strUserName = _T("xxx");
	m_strPassword = _T("1234");
	m_strServerIP = _T("127.0.0.1");
	m_nServerPort = 8888;
	UpdateData(FALSE);
}

void CZQQClientDlg::OnSendMessage()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	PublicChatMsg publicChatMsg;
	publicChatMsg.header.type = PUBLIC_CHAT_PKT;
	strcpy(publicChatMsg.from, (LPCTSTR)m_strUserName);
	strcpy(publicChatMsg.message, (LPCTSTR)m_strMessage);

	send(m_sClient, (const char*)&publicChatMsg, 4+20+m_strMessage.GetLength()+1, 0);
}
