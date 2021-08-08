
// 20170316_RemoteControlServer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy20170316_RemoteControlServerApp:
// See 20170316_RemoteControlServer.cpp for the implementation of this class
//

class CMy20170316_RemoteControlServerApp : public CWinApp
{
public:
	CMy20170316_RemoteControlServerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy20170316_RemoteControlServerApp theApp;