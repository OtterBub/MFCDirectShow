#pragma once

// {7FFE2C57-B873-4269-BB6B-3457EC1B5BDC}
DEFINE_GUID(CLSID_SKParkRendererFilter, 0x7ffe2c57, 0xb873, 0x4269, 0xbb, 0x6b, 0x34, 0x57, 0xec, 0x1b, 0x5b, 0xdc);

// {4598EFAE-68F6-4487-A181-7C276A7C859B}
DEFINE_GUID(IID_IVideoRenderer,
	0x4598efae, 0x68f6, 0x4487, 0xa1, 0x81, 0x7c, 0x27, 0x6a, 0x7c, 0x85, 0x9b);


enum DisplayMode
{
	KeepAspectRatio = 0,
	Fill = 1
};

DECLARE_INTERFACE_(IVideoRenderer, IUnknown)
{
	STDMETHOD(SetVideoWindow)(HWND hWnd) PURE;
	STDMETHOD_(void, SetDisplayMode)(DisplayMode) PURE;
	STDMETHOD_(DisplayMode, GetDisplayMode)(void) PURE;
};

#define FILTER_NAME TEXT("SKP-arkRendererFilter")