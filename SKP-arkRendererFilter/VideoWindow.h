#pragma once

#include <Windows.h>

class CVideoWindow
{
public:
	CVideoWindow(void);
	virtual ~CVideoWindow(void);

	int ShowWindowSync();
	HWND InitInstance(LPCWSTR windowName, int width, int height);

private:
	ATOM MyRegisterClass();

	HWND m_hWnd;
};