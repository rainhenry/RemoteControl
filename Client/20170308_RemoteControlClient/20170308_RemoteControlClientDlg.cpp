
// 20170308_RemoteControlClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include <cstdio>
#include "20170308_RemoteControlClient.h"
#include "20170308_RemoteControlClientDlg.h"
#include "afxdialogex.h"
#include "User.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy20170308_RemoteControlClientDlg dialog



CMy20170308_RemoteControlClientDlg::CMy20170308_RemoteControlClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy20170308_RemoteControlClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pUserWin = this;
}


CMy20170308_RemoteControlClientDlg::~CMy20170308_RemoteControlClientDlg()
{
}

void CMy20170308_RemoteControlClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mipedit);
	DDX_Control(pDX, IDC_EDIT2, mportedit);
	DDX_Control(pDX, IDOK, mConnectButton);
}

BEGIN_MESSAGE_MAP(CMy20170308_RemoteControlClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CMy20170308_RemoteControlClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMy20170308_RemoteControlClientDlg message handlers

BOOL CMy20170308_RemoteControlClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	TRACE("\r\n====================================CMy20170308_RemoteControlClientDlg::OnInitDialog\r\n");

	//	获取屏幕分辨率
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	
	//	初始化监听按键
	k1.AddKey(VK_F4);
	k1.Config(UserOnKey, &window_show_status);

	//	设置获取快捷键的定时器
	SetTimer(2,10,NULL);

	//	设置窗口的显示控制定时器
	SetTimer(3,100,NULL);


	//---------------------------------------------------------------------------------
	//	提示当前状态
	SetWindowText(_T("Ready"));

	//---------------------------------------------------------------------------------
	//	设置默认的IP和端口
	CString str_ip, str_port;
	str_ip.Format(_T("%S"), inet_ntoa(AppConfig.sin_addr));
	str_port.Format(_T("%d"), htons(AppConfig.sin_port));
	mipedit.SetWindowTextW(str_ip);
	mportedit.SetWindowTextW(str_port);
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy20170308_RemoteControlClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy20170308_RemoteControlClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy20170308_RemoteControlClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default


	switch (nIDEvent)
	{
	case 2:
		{
			  //	快捷键监听心跳
			  CKeyRecv::HeartbeatOnHandle();
		}
		break;

	case 3:
		{
			  //	如果为显示
			  if (window_show_status == true)
			  {
				  this->ShowWindow(SW_NORMAL);
			  }
			  //	否则为不显示
			  else
			  {
				  this->ShowWindow(SW_HIDE); 
			  }
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


//	用户定义的遍历函数
void Traverse(
	int x_dat,					//	图像的x值，横向值
	int y_dat,					//	图像的y值，纵向值
	DWORD rgb					//	RGB值，可以通过GetRVal等宏来提取RGB的颜色分量
	)
{
}

//	OK按钮
void CMy20170308_RemoteControlClientDlg::OnBnClickedOk()
{
	//	进入按钮响应
	mConnectButton_cs.Enter();
	this->mConnectButton.EnableWindow(false);

	//	如果为没有连接
	if (currect_connect_status == false)
	{
		//	执行连接过程
		//	获取输入的IP地址
		CString tmp_str;
		mipedit.GetWindowTextW(tmp_str);
		std::string ip_str = CWCharToChar::WChar_To_Char(tmp_str.GetBuffer());

		//	获取输入的端口
		mportedit.GetWindowTextW(tmp_str);
		std::string port_str = CWCharToChar::WChar_To_Char(tmp_str.GetBuffer());
		int port_value = 0;
		sscanf_s(((char*)port_str.data()), "%d", &port_value);

		//	保存配置参数
		AppConfig.sin_port = htons(port_value);
		AppConfig.sin_addr.S_un.S_addr = inet_addr(ip_str.data());
		myfilesave.WirteToFile();

		//	配置网络
		int re = myClient.Config(
			(char*)(ip_str.data()),
			port_value,
			true,
			OnRecvData,
			OnServerBreak,
			0,
			0
			);

		//	检查是否连接成功
		if (re == 0)
		{
			tmp_str.Format(_T("Connected at %S:%d(SOCKET:%d)"),
				ip_str.data(),
				port_value,
				myClient.GetSocket()
				);
			this->SetWindowTextW(tmp_str);

			//	连接成功
			currect_connect_status = true;

			//	设置按钮文字
			this->mConnectButton.SetWindowTextW(_T("Disconnect"));
		}
		//	否则连接失败
		else
		{
			this->SetWindowTextW(_T("Connect Fail!!"));

			//	连接失败
			currect_connect_status = false;

			//	设置按钮文字
			this->mConnectButton.SetWindowTextW(_T("Connenct"));
		}
	}
	//	如果为已经连接
	else
	{
		//	断开
		myClient.Disconnect();

		//	设置状态
		this->SetWindowTextW(_T("Ready"));

		//	连接失败
		currect_connect_status = false;

		//	设置按钮文字
		this->mConnectButton.SetWindowTextW(_T("Connenct"));
	}

	this->mConnectButton.EnableWindow(true);
	mConnectButton_cs.Leave();
}



