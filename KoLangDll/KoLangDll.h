// KoLangDll.h : main header file for the KoLangDll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKoLangDllApp
// See KoLangDll.cpp for the implementation of this class
//

class CKoLangDllApp : public CWinApp
{
public:
	CKoLangDllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
