
// MFCDirectShowTestAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCDirectShowTestApp.h"
#include "MFCDirectShowTestAppDlg.h"
#include "afxdialogex.h"

#include <dshow.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCDirectShowTestAppDlg dialog



CMFCDirectShowTestAppDlg::CMFCDirectShowTestAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCDIRECTSHOWTESTAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mfirstCreate = TRUE;
}

void CMFCDirectShowTestAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST2, m_CamList);
	DDX_Control(pDX, IDC_LIST3, m_ResList);
}

BEGIN_MESSAGE_MAP(CMFCDirectShowTestAppDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCDirectShowTestAppDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDCANCEL, &CMFCDirectShowTestAppDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT1, &CMFCDirectShowTestAppDlg::OnEnChangeEdit1)
	ON_LBN_SELCHANGE(IDC_LIST2, &CMFCDirectShowTestAppDlg::OnLbnSelchangeCamList)
	ON_STN_CLICKED(IDC_STATIC3, &CMFCDirectShowTestAppDlg::OnStnClickedStatic3)
	ON_LBN_SELCHANGE(IDC_LIST3, &CMFCDirectShowTestAppDlg::OnLbnSelchangeList3)
END_MESSAGE_MAP()


// CMFCDirectShowTestAppDlg message handlers

BOOL CMFCDirectShowTestAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	// DirectShow Init
	//m_cdshow.Initialize(GetDlgItem(IDC_STATIC)->m_hWnd);
	m_cdshow.Initialize(this->m_hWnd);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCDirectShowTestAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCDirectShowTestAppDlg::OnPaint()
{
	if (mfirstCreate)
	{	
		mfirstCreate = FALSE;
		m_cdshow.CameraStart();
	}
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
		CPaintDC dc(this); // device context for painting
		dc.Rectangle(10, 10, 100, 100);
		CDialogEx::OnPaint();
	}
}

afx_msg void CMFCDirectShowTestAppDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	AfxMessageBox(_T("OnCreate()"));

	CDialog::OnCreate(lpCreateStruct);
}

void CMFCDirectShowTestAppDlg::OnDestroy()
{
	m_cdshow.UnInitialize();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCDirectShowTestAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCDirectShowTestAppDlg::OnBnClickedApply()
{
	m_cdshow.CameraStop();
	m_cdshow.SetResolution(m_SelectRes);
	
	m_cdshow.CameraStart(m_SelectCamName);
	// CDialogEx::OnOK();
}


void CMFCDirectShowTestAppDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CMFCDirectShowTestAppDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CMFCDirectShowTestAppDlg::OnLbnSelchangeCamList()
{
	int index = m_CamList.GetCurSel();
	if (index < 0)
		return;
	
	m_CamList.GetText(index, m_SelectCamName);

	wprintf(L"CamList Index Change: %d \
			\nSelect Cam Name %s\n", index, m_SelectCamName);

	wprintf(L"This List Count: %d\n", m_CamList.GetCount());
	//wprintf(L"Select Cam Name %s\n", m_SelectCamName);
	// TODO: Add your control notification handler code here
	m_cdshow.SelectCaptureFilter(m_SelectCamName);
	m_cdshow.AddCamDeviceResolutionList();
}


void CMFCDirectShowTestAppDlg::OnStnClickedStatic3()
{
	// TODO: Add your control notification handler code here
}

void CMFCDirectShowTestAppDlg::OnLbnSelchangeList3()
{
	int index = m_ResList.GetCurSel();
	CString strRes;

	if (index < 0)
		return;
	
	m_ResList.GetText(index, strRes);

	m_SelectRes = strRes;
	wprintf(L"Select Res: %s\n", m_SelectRes);
	// TODO: Add your control notification handler code here
}
