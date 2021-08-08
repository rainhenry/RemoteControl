
// 20170308_RemoteControlClientDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CMy20170308_RemoteControlClientDlg dialog
class CMy20170308_RemoteControlClientDlg : public CDialogEx
{
// Construction
public:
	CMy20170308_RemoteControlClientDlg(CWnd* pParent = NULL);	// standard constructor
	~CMy20170308_RemoteControlClientDlg();

// Dialog Data
	enum { IDD = IDD_MY20170308_REMOTECONTROLCLIENT_DIALOG };

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedOk();

	CEdit mipedit;
	CEdit mportedit;
	CButton mConnectButton;
};
