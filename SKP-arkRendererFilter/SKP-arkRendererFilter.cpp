#include "pch.h"
#include "SKP-arkRendererFilter_Guids.h"
#include "SKP-arkRendererFilter.h"

CSKParkRender::CSKParkRender(LPUNKNOWN pUnk, HRESULT* phr)
	: CBaseVideoRenderer(CLSID_SKParkRendererFilter, FILTER_NAME, pUnk, phr),
	m_hWnd(0), m_converter(0)
{
	m_renderer = new CDRenderer();
	m_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

CSKParkRender::~CSKParkRender(void)
{
	delete m_converter;
	delete m_renderer;
	CloseHandle(m_event);
}

HRESULT CSKParkRender::DoRenderSample(IMediaSample *pMediaSample)
{
	BYTE* pBuffer = NULL;
	HR(pMediaSample->GetPointer(&pBuffer));

	if (m_mediaType.subtype != MEDIASUBTYPE_RGB32)
	{
		const BYTE* rgbaBuffer = m_converter->convert_to_rgb32(pBuffer);
		return m_renderer->DrawSample(rgbaBuffer);
	}

	return m_renderer->DrawSample(pBuffer);
}

HRESULT CSKParkRender::CheckMediaType(const CMediaType *pmt)
{
	CheckPointer(pmt, E_POINTER);

	if (pmt->majortype != MEDIATYPE_Video)
	{
		return E_FAIL;
	}

	const GUID *SubType = pmt->Subtype();
	if (SubType == NULL)
		return E_FAIL;

	if (*SubType != MEDIASUBTYPE_YV12 &&
		*SubType != WMMEDIASUBTYPE_I420 &&
		*SubType != MEDIASUBTYPE_IYUV &&
		*SubType != MEDIASUBTYPE_NV12 &&
		*SubType != MEDIASUBTYPE_YUY2 &&
		*SubType != MEDIASUBTYPE_RGB555 &&
		*SubType != MEDIASUBTYPE_RGB565 &&
		*SubType != MEDIASUBTYPE_RGB24 &&
		*SubType != MEDIASUBTYPE_RGB32)
	{
		return E_FAIL;
	}

	if (pmt->formattype != FORMAT_VideoInfo && pmt->formattype != FORMAT_VideoInfo2)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CSKParkRender::CreateDefaultWindow()
{
	CVideoWindow wnd;
	m_hWnd = wnd.InitInstance(L"Direct2D Video Window", m_bmpInfo.biWidth, m_bmpInfo.biHeight);
	::SetEvent(m_event);

	wnd.ShowWindowSync();
}

DWORD WINAPI RunWindowInBackgroundThread(LPVOID target)
{
	CSKParkRender* filter = (CSKParkRender*)target;
	if (filter != NULL)
	{
		filter->CreateDefaultWindow();
	}

	return 1;
}

HRESULT CSKParkRender::StartStreaming()
{
	::ShowWindowAsync(m_hWnd, SW_SHOW);

	return CBaseVideoRenderer::StartStreaming();
}

HRESULT CSKParkRender::SetMediaType(const CMediaType *pmt)
{
	CheckPointer(pmt, E_POINTER);
	CAutoLock m_lock(this->m_pLock);

	m_mediaType = *pmt;
	VIDEOINFOHEADER* vInfo = NULL;
	VIDEOINFOHEADER2* vInfo2 = NULL;

	if (pmt->formattype == FORMAT_VideoInfo)
	{
		vInfo = (VIDEOINFOHEADER*)pmt->pbFormat;
		if (vInfo == NULL || vInfo->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
		{
			return E_INVALIDARG;
		}
		m_bmpInfo = vInfo->bmiHeader;
	}
	else if (pmt->formattype == FORMAT_VideoInfo2)
	{
		vInfo2 = (VIDEOINFOHEADER2*)pmt->pbFormat;
		if (vInfo2 == NULL || vInfo2->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
		{
			return E_INVALIDARG;
		}
		m_bmpInfo = vInfo2->bmiHeader;
	}
	else
	{
		return E_INVALIDARG;
	}

	if (m_hWnd == NULL)
	{
		::CreateThread(NULL, 0, RunWindowInBackgroundThread, this, 0, 0);
		::WaitForSingleObject(m_event, INFINITE);
	}

	bool hFlip = (pmt->subtype == MEDIASUBTYPE_RGB24 || pmt->subtype == MEDIASUBTYPE_RGB32 ||
		pmt->subtype == MEDIASUBTYPE_RGB555 || pmt->subtype == MEDIASUBTYPE_RGB565);

	m_converter = new CColorSpaceConverter(pmt->subtype, m_bmpInfo.biWidth, m_bmpInfo.biHeight);

	return m_renderer->PrepareRenderTarget(m_bmpInfo, m_hWnd, hFlip);
}

CUnknown * WINAPI CSKParkRender::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	ASSERT(phr);

	CUnknown *punk = new CSKParkRender(lpunk, phr);
	if (punk == NULL)
	{
		if (phr)
			*phr = E_OUTOFMEMORY;
	}

	return punk;
}

HRESULT CSKParkRender::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IVideoRenderer)
	{
		return GetInterface((IVideoRenderer*)this, ppv);
	}

	return CBaseVideoRenderer::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CSKParkRender::SetVideoWindow(HWND hWnd)
{
	if (!IsWindow(hWnd))
	{
		return E_INVALIDARG;
	}

	m_hWnd = hWnd;
	return S_OK;
}

void CSKParkRender::SetDisplayMode(DisplayMode displayMode)
{
	m_renderer->SetDisplayMode(displayMode);
}

DisplayMode CSKParkRender::GetDisplayMode()
{
	return m_renderer->GetDisplayMode();
}