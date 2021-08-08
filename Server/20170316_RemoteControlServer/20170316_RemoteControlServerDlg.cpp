
// 20170316_RemoteControlServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "20170316_RemoteControlServer.h"
#include "20170316_RemoteControlServerDlg.h"
#include "afxdialogex.h"
#include "User.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy20170316_RemoteControlServerDlg dialog



CMy20170316_RemoteControlServerDlg::CMy20170316_RemoteControlServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy20170316_RemoteControlServerDlg::IDD, pParent)
{
	pUserMainWindow = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//	增加窗口析构函数，主要用来释放OpenCV的图像数据，和销毁窗口
CMy20170316_RemoteControlServerDlg::~CMy20170316_RemoteControlServerDlg()
{
	//	检查图像指针是否需要释放
	pScreenshort_cs.Enter();
	if (pScreenshort != NULL)
	{
		cvReleaseImage(&pScreenshort);
		pScreenshort = NULL;
	}
	pScreenshort_cs.Leave();

	//	销毁窗口
	cvDestroyWindow(REMOTE_DESKTOP_WINDOW_NAME);
}

void CMy20170316_RemoteControlServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mCurrentSelectEdit);
	DDX_Control(pDX, IDC_RADIO1, mAllNetworkCard);
	DDX_Control(pDX, IDC_RADIO2, mSelectNetworkCard);
	DDX_Control(pDX, IDC_IPADDRESS1, mIP);
	DDX_Control(pDX, IDC_EDIT2, mPort);
	DDX_Control(pDX, IDOK, mRunButton);
	DDX_Control(pDX, IDC_LIST1, mClientList);
	DDX_Control(pDX, IDC_BUTTON1, mMonitorButton);
	DDX_Control(pDX, IDC_EDIT3, mFPSedit);
}

BEGIN_MESSAGE_MAP(CMy20170316_RemoteControlServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMy20170316_RemoteControlServerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO1, &CMy20170316_RemoteControlServerDlg::OnBnClickedAllNetworkCard)
	ON_BN_CLICKED(IDC_RADIO2, &CMy20170316_RemoteControlServerDlg::OnBnClickedSelectNetworkCard)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CMy20170316_RemoteControlServerDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CMy20170316_RemoteControlServerDlg::OnClickList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CMy20170316_RemoteControlServerDlg::OnBnClickedButton1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMy20170316_RemoteControlServerDlg message handlers

BOOL CMy20170316_RemoteControlServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//--------------------------------------------------------------------------
	//	提示OpenCV版本
	TRACE("\r\n-----------------------OpenCV Version:%s -------------------\r\n", CV_VERSION);

	//--------------------------------------------------------------------------
	//	设置默认的网卡选择
	this->mAllNetworkCard.SetCheck(1);
	this->mSelectNetworkCard.SetCheck(0);
	this->mIP.EnableWindow(false);

	//--------------------------------------------------------------------------
	//	设置默认端口
	this->mPort.SetWindowTextW(_T("1234"));

	//--------------------------------------------------------------------------
	//	设置选择文本
	UserInterface_CurrentClientEdit_CS.Enter();
	this->mCurrentSelectEdit.SetWindowTextW(_T("Please select client"));
	UserInterface_CurrentClientEdit_CS.Leave();

	//--------------------------------------------------------------------------
	//	配置类表框
	UserInterface_ClientList_CS.Enter();
	this->mClientList.SetExtendedStyle(this->mClientList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	this->mClientList.InsertColumn(0, _T("IP"),				LVCFMT_CENTER,	120,	0);
	this->mClientList.InsertColumn(1, _T("Port"),			LVCFMT_CENTER,	80,		1);
	this->mClientList.InsertColumn(2, _T("SOCKET"),			LVCFMT_CENTER,	80,		2);
	this->mClientList.InsertColumn(3, _T("Begin Time"),		LVCFMT_CENTER,	220,	3);
	UserInterface_ClientList_CS.Leave();
	
	//--------------------------------------------------------------------------
	//	配置发送线程函数
	SendCtrlThread.Config(
		SendCtrlThreadFunc,
		NULL,
		CThread::STOP
		);

	//--------------------------------------------------------------------------
	//	配置定时器
	SetTimer(1, 100, NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy20170316_RemoteControlServerDlg::OnPaint()
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
HCURSOR CMy20170316_RemoteControlServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//	运行按钮
void CMy20170316_RemoteControlServerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
	////int i = this->mClientList.GetSelectionMark();
	//this->mClientList.InsertItem(0, _T("192.168.224.222"));
	//this->mClientList.SetItemText(0, 1, _T("23456"));
	//this->mClientList.SetItemText(0, 2, _T("518"));
	//CMyTime now_time; now_time.GetNowTime();
	//this->mClientList.SetItemText(0, 3, CWCharToChar::Char_To_WChar(now_time.GetTimeString()).data());
	////this->mClientList.SetItemState(i+1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	////this->mClientList.SetSelectionMark(i+1);
	//CString str;
	//str.Format(_T("%d"), this->mClientList.GetItemCount());
	//MessageBox(str);

	//	根据当前的工作状态进行配置,如果服务器没有工作
	if (TCPServerStatus == false)
	{
		//	设置按钮状态
		this->mRunButton.SetWindowTextW(_T("Stop Server"));

		//	获得当前端口号
		int port_value = 0;
		CString port_str;
		this->mPort.GetWindowTextW(port_str);
		swscanf_s(port_str, _T("%d"), &port_value);

		//	根据网卡的选择进行初始化配置进行配置资源,如果当前选择了全部网卡
		if (this->mAllNetworkCard.GetCheck() != 0)
		{
			myTCPServerRes.Config(
				port_value
				);
		}
		//	否则选择的是指定网卡
		else
		{
			//	获取指定网卡的IP
			CUDPServer myudp;
			std::string ip_str = myudp.GetIPStrFromCIPAddressCtrl(&(this->mIP));
			myTCPServerRes.Config(
				ip_str.data(),
				port_value
				);
		}

		//	配置网络管理类
		myTCPManage.Config(
			myTCPServerRes,
			OnRecvUserData,
			OnTCPJoin,
			OnTCPBreak
			);

		//	设置已经工作标志
		TCPServerStatus = true;
	}
	//	否则服务器正在工作
	else
	{
		//	设置按钮状态
		this->mRunButton.SetWindowTextW(_T("Run Server"));

		//	停止服务器
		myTCPServerRes.Release();

		//	删除全部链接
		RemoveAllClient();

		//	设置选择文本
		UserInterface_CurrentClientEdit_CS.Enter();
		this->mCurrentSelectEdit.SetWindowTextW(_T("Please select client"));
		UserInterface_CurrentClientEdit_CS.Leave();

		//	设置没有工作标志
		TCPServerStatus = false;

		//	清空FPS显示
		SetFPS(0.0);
	}

}

//	单机全部网卡
void CMy20170316_RemoteControlServerDlg::OnBnClickedAllNetworkCard()
{
	//	取消指定网卡的选择
	this->mSelectNetworkCard.SetCheck(0);
	this->mIP.EnableWindow(false);
}

//	单击指定网卡
void CMy20170316_RemoteControlServerDlg::OnBnClickedSelectNetworkCard()
{
	//	取消全部网卡的选择
	this->mAllNetworkCard.SetCheck(0);
	this->mIP.EnableWindow(true);
}



void CMy20170316_RemoteControlServerDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

//	当单击类表控件的时候
void CMy20170316_RemoteControlServerDlg::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	//	当监控没有开始的时候
	if (MonitorStatus == false)
	{
		//	当单机客户端列表的时候，在选择文本中列出主要客户端信息
		CString tmp_str;
		UserInterface_ClientList_CS.Enter();
		tmp_str.Format(_T("Already selected:  %s:%s(SOCKET:%s)"),
			this->mClientList.GetItemText(this->mClientList.GetSelectionMark(), 0),
			this->mClientList.GetItemText(this->mClientList.GetSelectionMark(), 1),
			this->mClientList.GetItemText(this->mClientList.GetSelectionMark(), 2)
			);
		UserInterface_ClientList_CS.Leave();
		UserInterface_CurrentClientEdit_CS.Enter();
		this->mCurrentSelectEdit.SetWindowTextW(tmp_str);
		UserInterface_CurrentClientEdit_CS.Leave();

		//	设置信息
		GetCurrentSelectClientInfo(&CurrentClientInfo);
	}

	*pResult = 0;
}

//	单击监控按钮的响应
void CMy20170316_RemoteControlServerDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	//	如果当前监控没有工作
	MonitorAboutVar_CS.Enter();

	//	20170331，修订程序，优化按钮操作，忙的时候禁止使用按钮
	//	禁止按钮
	this->mMonitorButton.EnableWindow(false);

	if (MonitorStatus == false)
	{
		//	创建OpenCV窗口
		cvNamedWindow(REMOTE_DESKTOP_WINDOW_NAME, CV_WINDOW_AUTOSIZE);

		//	启动线程
		SendCtrlThread.Run();

		//	设置脱离
		this->mMonitorButton.SetWindowTextW(_T("Escape"));

		//	开始工作
		MonitorStatus = true;
	}
	//	当前监控已经工作
	else
	{
		//	关闭线程
		SendCtrlThread.Stop();
		Sleep(500);

		//	关闭窗口
		cvDestroyWindow(REMOTE_DESKTOP_WINDOW_NAME);

		//	释放图像
		pScreenshort_cs.Enter();
		if (pScreenshort != NULL)
		{
			cvReleaseImage(&pScreenshort);
			pScreenshort = NULL;
		}
		pScreenshort_cs.Leave();

		//	设置监控
		this->mMonitorButton.SetWindowTextW(_T("Monitor"));

		//	停止工作
		MonitorStatus = false;

		//	清空FPS显示
		SetFPS(0.0);
	}

	//	使能按钮
	this->mMonitorButton.EnableWindow(true);
	MonitorAboutVar_CS.Leave();
}


void CMy20170316_RemoteControlServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	//	进入FPS更新
	if (nIDEvent == 1)
	{
		CString tmp_str;
		tmp_str.Format(_T("%0.2lf"), GetFPS());
		this->mFPSedit.SetWindowTextW(tmp_str);
	}

	CDialogEx::OnTimer(nIDEvent);
}
