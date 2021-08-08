
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

	//	��ȡ��Ļ�ֱ���
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	
	//	��ʼ����������
	k1.AddKey(VK_F4);
	k1.Config(UserOnKey, &window_show_status);

	//	���û�ȡ��ݼ��Ķ�ʱ��
	SetTimer(2,10,NULL);

	//	���ô��ڵ���ʾ���ƶ�ʱ��
	SetTimer(3,100,NULL);


	//---------------------------------------------------------------------------------
	//	��ʾ��ǰ״̬
	SetWindowText(_T("Ready"));

	//---------------------------------------------------------------------------------
	//	����Ĭ�ϵ�IP�Ͷ˿�
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
			  //	��ݼ���������
			  CKeyRecv::HeartbeatOnHandle();
		}
		break;

	case 3:
		{
			  //	���Ϊ��ʾ
			  if (window_show_status == true)
			  {
				  this->ShowWindow(SW_NORMAL);
			  }
			  //	����Ϊ����ʾ
			  else
			  {
				  this->ShowWindow(SW_HIDE); 
			  }
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


//	�û�����ı�������
void Traverse(
	int x_dat,					//	ͼ���xֵ������ֵ
	int y_dat,					//	ͼ���yֵ������ֵ
	DWORD rgb					//	RGBֵ������ͨ��GetRVal�Ⱥ�����ȡRGB����ɫ����
	)
{
}

//	OK��ť
void CMy20170308_RemoteControlClientDlg::OnBnClickedOk()
{
	//	���밴ť��Ӧ
	mConnectButton_cs.Enter();
	this->mConnectButton.EnableWindow(false);

	//	���Ϊû������
	if (currect_connect_status == false)
	{
		//	ִ�����ӹ���
		//	��ȡ�����IP��ַ
		CString tmp_str;
		mipedit.GetWindowTextW(tmp_str);
		std::string ip_str = CWCharToChar::WChar_To_Char(tmp_str.GetBuffer());

		//	��ȡ����Ķ˿�
		mportedit.GetWindowTextW(tmp_str);
		std::string port_str = CWCharToChar::WChar_To_Char(tmp_str.GetBuffer());
		int port_value = 0;
		sscanf_s(((char*)port_str.data()), "%d", &port_value);

		//	�������ò���
		AppConfig.sin_port = htons(port_value);
		AppConfig.sin_addr.S_un.S_addr = inet_addr(ip_str.data());
		myfilesave.WirteToFile();

		//	��������
		int re = myClient.Config(
			(char*)(ip_str.data()),
			port_value,
			true,
			OnRecvData,
			OnServerBreak,
			0,
			0
			);

		//	����Ƿ����ӳɹ�
		if (re == 0)
		{
			tmp_str.Format(_T("Connected at %S:%d(SOCKET:%d)"),
				ip_str.data(),
				port_value,
				myClient.GetSocket()
				);
			this->SetWindowTextW(tmp_str);

			//	���ӳɹ�
			currect_connect_status = true;

			//	���ð�ť����
			this->mConnectButton.SetWindowTextW(_T("Disconnect"));
		}
		//	��������ʧ��
		else
		{
			this->SetWindowTextW(_T("Connect Fail!!"));

			//	����ʧ��
			currect_connect_status = false;

			//	���ð�ť����
			this->mConnectButton.SetWindowTextW(_T("Connenct"));
		}
	}
	//	���Ϊ�Ѿ�����
	else
	{
		//	�Ͽ�
		myClient.Disconnect();

		//	����״̬
		this->SetWindowTextW(_T("Ready"));

		//	����ʧ��
		currect_connect_status = false;

		//	���ð�ť����
		this->mConnectButton.SetWindowTextW(_T("Connenct"));
	}

	this->mConnectButton.EnableWindow(true);
	mConnectButton_cs.Leave();
}



