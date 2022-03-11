#pragma once

#include <streams.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <atlbase.h>

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

template <typename T>
inline void SafeRelease(T& p)
{
	if (NULL != p)
	{
		p.Release();
		p = NULL;
	}
}

#define HR(x) if(FAILED(x)) { return x; }

class CDRenderer
{
public:
	CDRenderer(void);
	virtual ~CDRenderer(void);

	HRESULT PrepareRenderTarget(BITMAPINFOHEADER& bih, HWND hWnd, bool bFlipHorizontally);
	HRESULT DrawSample(const BYTE* pRgb32Buffer);
	void SetDisplayMode(DisplayMode mode);
	DisplayMode GetDisplayMode();

protected:
	HRESULT CreateResources();
	void DiscardResources();

protected:
	CComPtr<ID2D1Factory> m_d2dFactory;
	CComPtr<ID2D1HwndRenderTarget> m_hWndTarget;
	CComPtr<ID2D1Bitmap> m_bitmap;

	UINT32 m_pitch;
	DisplayMode m_displayMode;
	BITMAPINFOHEADER m_pBitmapInfo;
	bool m_bFlipHorizontally;
	HWND m_hWnd;
};