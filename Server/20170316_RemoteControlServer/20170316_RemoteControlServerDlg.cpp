
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

//	���Ӵ���������������Ҫ�����ͷ�OpenCV��ͼ�����ݣ������ٴ���
CMy20170316_RemoteControlServerDlg::~CMy20170316_RemoteControlServerDlg()
{
	//	���ͼ��ָ���Ƿ���Ҫ�ͷ�
	pScreenshort_cs.Enter();
	if (pScreenshort != NULL)
	{
		cvReleaseImage(&pScreenshort);
		pScreenshort = NULL;
	}
	pScreenshort_cs.Leave();

	//	���ٴ���
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
	//	��ʾOpenCV�汾
	TRACE("\r\n-----------------------OpenCV Version:%s -------------------\r\n", CV_VERSION);

	//--------------------------------------------------------------------------
	//	����Ĭ�ϵ�����ѡ��
	this->mAllNetworkCard.SetCheck(1);
	this->mSelectNetworkCard.SetCheck(0);
	this->mIP.EnableWindow(false);

	//--------------------------------------------------------------------------
	//	����Ĭ�϶˿�
	this->mPort.SetWindowTextW(_T("1234"));

	//--------------------------------------------------------------------------
	//	����ѡ���ı�
	UserInterface_CurrentClientEdit_CS.Enter();
	this->mCurrentSelectEdit.SetWindowTextW(_T("Please select client"));
	UserInterface_CurrentClientEdit_CS.Leave();

	//--------------------------------------------------------------------------
	//	��������
	UserInterface_ClientList_CS.Enter();
	this->mClientList.SetExtendedStyle(this->mClientList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	this->mClientList.InsertColumn(0, _T("IP"),				LVCFMT_CENTER,	120,	0);
	this->mClientList.InsertColumn(1, _T("Port"),			LVCFMT_CENTER,	80,		1);
	this->mClientList.InsertColumn(2, _T("SOCKET"),			LVCFMT_CENTER,	80,		2);
	this->mClientList.InsertColumn(3, _T("Begin Time"),		LVCFMT_CENTER,	220,	3);
	UserInterface_ClientList_CS.Leave();
	
	//--------------------------------------------------------------------------
	//	���÷����̺߳���
	SendCtrlThread.Config(
		SendCtrlThreadFunc,
		NULL,
		CThread::STOP
		);

	//--------------------------------------------------------------------------
	//	���ö�ʱ��
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

//	���а�ť
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

	//	���ݵ�ǰ�Ĺ���״̬��������,���������û�й���
	if (TCPServerStatus == false)
	{
		//	���ð�ť״̬
		this->mRunButton.SetWindowTextW(_T("Stop Server"));

		//	��õ�ǰ�˿ں�
		int port_value = 0;
		CString port_str;
		this->mPort.GetWindowTextW(port_str);
		swscanf_s(port_str, _T("%d"), &port_value);

		//	����������ѡ����г�ʼ�����ý���������Դ,�����ǰѡ����ȫ������
		if (this->mAllNetworkCard.GetCheck() != 0)
		{
			myTCPServerRes.Config(
				port_value
				);
		}
		//	����ѡ�����ָ������
		else
		{
			//	��ȡָ��������IP
			CUDPServer myudp;
			std::string ip_str = myudp.GetIPStrFromCIPAddressCtrl(&(this->mIP));
			myTCPServerRes.Config(
				ip_str.data(),
				port_value
				);
		}

		//	�������������
		myTCPManage.Config(
			myTCPServerRes,
			OnRecvUserData,
			OnTCPJoin,
			OnTCPBreak
			);

		//	�����Ѿ�������־
		TCPServerStatus = true;
	}
	//	������������ڹ���
	else
	{
		//	���ð�ť״̬
		this->mRunButton.SetWindowTextW(_T("Run Server"));

		//	ֹͣ������
		myTCPServerRes.Release();

		//	ɾ��ȫ������
		RemoveAllClient();

		//	����ѡ���ı�
		UserInterface_CurrentClientEdit_CS.Enter();
		this->mCurrentSelectEdit.SetWindowTextW(_T("Please select client"));
		UserInterface_CurrentClientEdit_CS.Leave();

		//	����û�й�����־
		TCPServerStatus = false;

		//	���FPS��ʾ
		SetFPS(0.0);
	}

}

//	����ȫ������
void CMy20170316_RemoteControlServerDlg::OnBnClickedAllNetworkCard()
{
	//	ȡ��ָ��������ѡ��
	this->mSelectNetworkCard.SetCheck(0);
	this->mIP.EnableWindow(false);
}

//	����ָ������
void CMy20170316_RemoteControlServerDlg::OnBnClickedSelectNetworkCard()
{
	//	ȡ��ȫ��������ѡ��
	this->mAllNetworkCard.SetCheck(0);
	this->mIP.EnableWindow(true);
}



void CMy20170316_RemoteControlServerDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

//	���������ؼ���ʱ��
void CMy20170316_RemoteControlServerDlg::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	//	�����û�п�ʼ��ʱ��
	if (MonitorStatus == false)
	{
		//	�������ͻ����б��ʱ����ѡ���ı����г���Ҫ�ͻ�����Ϣ
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

		//	������Ϣ
		GetCurrentSelectClientInfo(&CurrentClientInfo);
	}

	*pResult = 0;
}

//	������ذ�ť����Ӧ
void CMy20170316_RemoteControlServerDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	//	�����ǰ���û�й���
	MonitorAboutVar_CS.Enter();

	//	20170331���޶������Ż���ť������æ��ʱ���ֹʹ�ð�ť
	//	��ֹ��ť
	this->mMonitorButton.EnableWindow(false);

	if (MonitorStatus == false)
	{
		//	����OpenCV����
		cvNamedWindow(REMOTE_DESKTOP_WINDOW_NAME, CV_WINDOW_AUTOSIZE);

		//	�����߳�
		SendCtrlThread.Run();

		//	��������
		this->mMonitorButton.SetWindowTextW(_T("Escape"));

		//	��ʼ����
		MonitorStatus = true;
	}
	//	��ǰ����Ѿ�����
	else
	{
		//	�ر��߳�
		SendCtrlThread.Stop();
		Sleep(500);

		//	�رմ���
		cvDestroyWindow(REMOTE_DESKTOP_WINDOW_NAME);

		//	�ͷ�ͼ��
		pScreenshort_cs.Enter();
		if (pScreenshort != NULL)
		{
			cvReleaseImage(&pScreenshort);
			pScreenshort = NULL;
		}
		pScreenshort_cs.Leave();

		//	���ü��
		this->mMonitorButton.SetWindowTextW(_T("Monitor"));

		//	ֹͣ����
		MonitorStatus = false;

		//	���FPS��ʾ
		SetFPS(0.0);
	}

	//	ʹ�ܰ�ť
	this->mMonitorButton.EnableWindow(true);
	MonitorAboutVar_CS.Leave();
}


void CMy20170316_RemoteControlServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	//	����FPS����
	if (nIDEvent == 1)
	{
		CString tmp_str;
		tmp_str.Format(_T("%0.2lf"), GetFPS());
		this->mFPSedit.SetWindowTextW(tmp_str);
	}

	CDialogEx::OnTimer(nIDEvent);
}
