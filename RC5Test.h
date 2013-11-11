// RC5Test.h : main header file for the RC5Test application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CRC5TestApp:
// See RC5Test.cpp for the implementation of this class
//

class CRC5TestApp : public CWinApp
{
public:
	CRC5TestApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CRC5TestApp theApp;