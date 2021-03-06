
// MFCDirectShowTestApp.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "MFCDirectShowTestApp.h"
#include "MFCDirectShowTestAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCDirectShowTestApp

BEGIN_MESSAGE_MAP(CMFCDirectShowTestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFCDirectShowTestApp construction

CMFCDirectShowTestApp::CMFCDirectShowTestApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFCDirectShowTestApp object

CMFCDirectShowTestApp theApp;


// CMFCDirectShowTestApp initialization

BOOL CMFCDirectShowTestApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// Lang DLL Test Line
	// Load DLL Image Test
	TCHAR buff[MAX_PATH];

	// Load DLL
	HINSTANCE hInst = NULL, currentInst = NULL;
	CString cstrPath;
	TCHAR path[MAX_PATH] = { 0, };

	// Get Current Hinstance
	currentInst = AfxGetResourceHandle();

	// Get Directory by Current Execute File
	GetModuleFileName(currentInst, path, MAX_PATH);
	PathRemoveFileSpec(path);
	cstrPath.SetString(path);

	// Load Language DLL
	//cstrPath.Append(L"\\KoLangDll.dll");
	// hInst = LoadLibrary(cstrPath);

	// Set DLL Resource
	if (hInst)
		AfxSetResourceHandle(hInst);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;


	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CMFCDirectShowTestAppDlg dlg;
	
	INT_PTR nResponse = dlg.DoModal();

	
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}