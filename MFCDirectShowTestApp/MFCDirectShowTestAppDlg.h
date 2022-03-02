
// MFCDirectShowTestAppDlg.h : header file
//

#pragma once

#include "CDShow.h"

// CMFCDirectShowTestAppDlg dialog
class CMFCDirectShowTestAppDlg : public CDialogEx
{
// Construction
public:
	CMFCDirectShowTestAppDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCDIRECTSHOWTESTAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	// DirectShow Custom Class
	CDShow m_cdshow;
	
	// View Window
	CStatic m_StaticWnd;

	// ListBox
	CListBox m_ResList;
	CListBox m_CamList;
	CListBox m_ScaleList;

	// Current Select String (ListBox)
	CString m_SelectCamName;
	CString m_SelectRes;
	CBitmapButton m_CBitmapBtn;
	CEdit m_ReadReg;

public:
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnLbnSelchangeCamList();
	afx_msg void OnStnClickedStatic3();
	afx_msg void OnLbnSelchangeList3();
	afx_msg void OnLbnSelchangeList4();
	afx_msg void OnBnClickedButton1();
};
