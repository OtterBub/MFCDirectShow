#pragma once

// {7FFE2C57-B873-4269-BB6B-3457EC1B5BDC}
DEFINE_GUID(CLSID_SKParkRendererFilter, 0x7ffe2c57, 0xb873, 0x4269, 0xbb, 0x6b, 0x34, 0x57, 0xec, 0x1b, 0x5b, 0xdc);

static const GUID IID_IVideoRenderer =
{ 0x34e5b77c, 0xccba, 0x4ec0, { 0x88, 0xb5, 0xba, 0xbf, 0x6c, 0xf3, 0xa1, 0xd2 } };

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