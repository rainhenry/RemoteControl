
// 20170308_RemoteControlClient.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy20170308_RemoteControlClientApp:
// See 20170308_RemoteControlClient.cpp for the implementation of this class
//

class CMy20170308_RemoteControlClientApp : public CWinApp
{
public:
	CMy20170308_RemoteControlClientApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy20170308_RemoteControlClientApp theApp;