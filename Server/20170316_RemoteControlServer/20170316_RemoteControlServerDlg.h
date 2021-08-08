
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
	// ��ǰѡ������ӵ��ı���
	CEdit mCurrentSelectEdit;
	// ȫ��������IP
	CButton mAllNetworkCard;
	// ָ��������IP
	CButton mSelectNetworkCard;
	// IP����ؼ�
	CIPAddressCtrl mIP;
	// �˿�����ؼ�
	CEdit mPort;
	// ���а�ť
	CButton mRunButton;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAllNetworkCard();
	afx_msg void OnBnClickedSelectNetworkCard();
	// �����ӿͻ��˵��б�
	CListCtrl mClientList;
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	// ��ذ�ť
	CButton mMonitorButton;
	afx_msg void OnBnClickedButton1();
	CEdit mFPSedit;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
