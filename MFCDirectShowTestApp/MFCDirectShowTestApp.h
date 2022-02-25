
// MFCDirectShowTestApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFCDirectShowTestApp:
// See MFCDirectShowTestApp.cpp for the implementation of this class
//

class CMFCDirectShowTestApp : public CWinApp
{
public:
	CMFCDirectShowTestApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMFCDirectShowTestApp theApp;
