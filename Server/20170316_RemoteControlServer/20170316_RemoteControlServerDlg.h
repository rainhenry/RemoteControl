
// 20170316_RemoteControlServerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMy20170316_RemoteControlServerDlg dialog
class CMy20170316_RemoteControlServerDlg : public CDialogEx
{
// Construction
public:
	CMy20170316_RemoteControlServerDlg(CWnd* pParent = NULL);	// standard constructor
	~CMy20170316_RemoteControlServerDlg();

// Dialog Data
	enum { IDD = IDD_MY20170316_REMOTECONTROLSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 当前选择的连接的文本框
	CEdit mCurrentSelectEdit;
	// 全部的网卡IP
	CButton mAllNetworkCard;
	// 指定的网卡IP
	CButton mSelectNetworkCard;
	// IP输入控件
	CIPAddressCtrl mIP;
	// 端口输入控件
	CEdit mPort;
	// 运行按钮
	CButton mRunButton;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAllNetworkCard();
	afx_msg void OnBnClickedSelectNetworkCard();
	// 已连接客户端的列表
	CListCtrl mClientList;
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	// 监控按钮
	CButton mMonitorButton;
	afx_msg void OnBnClickedButton1();
	CEdit mFPSedit;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
