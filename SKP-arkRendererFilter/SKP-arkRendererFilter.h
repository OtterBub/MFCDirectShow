#pragma once

#include <dvdmedia.h>
#include "DRenderer.h"
#include "D3dRenderer.h"
#include "VideoWindow.h"
#include "ColorSpaceConverter.h"
#include "SKP-arkRendererFilter_Guids.h"


class CSKParkInputPin;
class CSKPark;
class CSKParkFilter;

#define BYTES_PER_LINE 20
#define FIRST_HALF_LINE TEXT  ("   %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x")
#define SECOND_HALF_LINE TEXT (" %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x")

// Renderer Filter
class CSKParkRender : public CBaseVideoRenderer, public IVideoRenderer
{
public:
	DECLARE_IUNKNOWN;
	
	CSKParkRender(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CSKParkRender(void);

	virtual HRESULT DoRenderSample(IMediaSample *pMediaSample);
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
	virtual HRESULT SetMediaType(const CMediaType *pmt);
	virtual HRESULT StartStreaming();

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	STDMETHODIMP SetVideoWindow(HWND hWnd);
	STDMETHOD_(void, SetDisplayMode)(DisplayMode);
	STDMETHOD_(DisplayMode, GetDisplayMode)(void);
	STDMETHOD_(void, SetDrawSDKMode)(DrawSDKMode);

	void CreateDefaultWindow();


protected:
	HWND m_hWnd;
	CDRenderer* m_renderer;
	HANDLE m_event;
	BITMAPINFOHEADER m_bmpInfo;
	CMediaType m_mediaType;
	CColorSpaceConverter* m_converter;

	DrawSDKMode m_SDKMode;

	CD3Drenderer* m_3drenderer;

private:
};