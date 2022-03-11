#include "pch.h"
#include "VideoWindow.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

CVideoWindow::CVideoWindow(void)
	: m_hWnd(0)
{
	MyRegisterClass();
}


CVideoWindow::~CVideoWindow(void)
{
}

int CVideoWindow::ShowWindowSync()
{
	MSG msg;

	ShowWindow(m_hWnd, SW_HIDE);
	UpdateWindow(m_hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

ATOM CVideoWindow::MyRegisterClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MyWindow";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

HWND CVideoWindow::InitInstance(LPCWSTR windowName, int width, int height)
{
	int x = ::GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	int y = ::GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

	m_hWnd = CreateWindow(L"MyWindow", windowName, WS_OVERLAPPEDWINDOW, x, y, width, height, NULL, NULL, NULL, NULL);

	return m_hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
