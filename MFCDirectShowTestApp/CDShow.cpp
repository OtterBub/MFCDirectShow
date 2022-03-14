#include "pch.h"
#include "CDShow.h"

// ----- public -----

CDShow::CDShow() 
{
	m_pGraph = NULL;
	m_pCaptureBuilder = NULL;
	m_pMC = NULL;
	m_pVW = NULL;
	m_pME = NULL;
	m_scale = 1;
}

CDShow::~CDShow() 
{
	SAFE_RELEASE(m_pGraph);
	SAFE_RELEASE(m_pCaptureBuilder);
	//SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW);
	SAFE_RELEASE(m_pME);
}

HRESULT CDShow::Initialize()
{
	HRESULT hr = S_OK;
	hr = CoInitialize(NULL);
	RETURN_FAILD_HRESULT(hr);

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraph);
	RETURN_FAILD_HRESULT(hr);

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&m_pCaptureBuilder);
	RETURN_FAILD_HRESULT(hr);

	hr = CoCreateInstance(CLSID_SKParkRendererFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pRenderFilter);
	RETURN_FAILD_HRESULT(hr);

	CComPtr<IBaseFilter> mjpegDec;
	hr = CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&mjpegDec);
	
	hr = m_pCaptureBuilder->SetFiltergraph(m_pGraph);
	RETURN_FAILD_HRESULT(hr);

	hr = m_pGraph->AddFilter(m_pRenderFilter, NULL);
	RETURN_FAILD_HRESULT(hr);

	hr = m_pGraph->AddFilter(mjpegDec, NULL);
	RETURN_FAILD_HRESULT(hr);

	m_pRenderFilter->QueryInterface(IID_IVideoRenderer, (void**)&m_pVideoRenderer);
	RETURN_FAILD_HRESULT(hr);

	hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pMC);
	RETURN_FAILD_HRESULT(hr);
	
	hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&m_pVW);
	RETURN_FAILD_HRESULT(hr);
	
	hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_pME);
	RETURN_FAILD_HRESULT(hr);

	

	InitCamDeviceList();
	SelectCaptureFilter();

	return hr;
}

HRESULT CDShow::Initialize(HWND hwnd)
{
	HRESULT hr = S_OK;
	RECT wrc;
	GetWindowRect(hwnd, &wrc);

	m_hWndDraw = hwnd;

	hr = CDShow::Initialize();
	
	if(m_pVideoRenderer)
		m_pVideoRenderer->SetVideoWindow(GetDlgItem(m_hWndDraw, IDC_STATIC3));
	
	RETURN_FAILD_HRESULT(hr);

	return hr;
}

HRESULT CDShow::UnInitialize()
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_pGraph);
	SAFE_RELEASE(m_pCaptureBuilder);
	//SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW);
	SAFE_RELEASE(m_pME);

	//SAFE_RELEASE(m_pVideoRenderer);

	for each (std::pair<CString, IBaseFilter*> var in m_mapCamDevicesFilter) {
		SAFE_RELEASE(var.second);
	}

	// Warning: dont Uninitialize before RELEASE
	// -> Error Occur
	CoUninitialize();	

	return hr;
}

std::vector<VARIANT> CDShow::GetCamDeviceList() 
{
	return m_vecCamDevicesList;
}

HRESULT CDShow::CameraStart(CString deviceFriendlyName)
{
	HRESULT hr = S_OK;

	
	// If L""
	if (deviceFriendlyName == L"" &&
		(m_mapCamDevicesFilter.size() > 0)) {
		m_pCurrentCaptureFilter = m_mapCamDevicesFilter.begin()->second;
	}
	// Searching By deviceFriendlyName
	else if (m_mapCamDevicesFilter.count(deviceFriendlyName) &&
		(m_mapCamDevicesFilter.size() > 0))
		m_pCurrentCaptureFilter = m_mapCamDevicesFilter.find(deviceFriendlyName)->second;
	else
		return E_FAIL;

	hr = m_pGraph->AddFilter(m_pCurrentCaptureFilter, NULL);
	RETURN_FAILD_HRESULT(hr);

	//hr = m_pCaptureBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pCurrentCaptureFilter, NULL, NULL);
	hr = m_pCaptureBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pCurrentCaptureFilter, NULL, m_pRenderFilter);
	RETURN_FAILD_HRESULT(hr);

	CameraSetWindow(GetDlgItem(m_hWndDraw, IDC_STATIC3));

	m_pMC->Run();
	
	return hr;
}

HRESULT CDShow::CameraStop()
{
	HRESULT hr = S_OK;

	// 카메라 정지
	hr = m_pMC->Stop();
	RETURN_FAILD_HRESULT(hr);

	// 현재 필터 제거
	hr = m_pGraph->RemoveFilter(m_pCurrentCaptureFilter);
	RETURN_FAILD_HRESULT(hr);

	return hr;
}

HRESULT CDShow::AddCamDeviceResolutionList()
{
	HRESULT hr = S_OK;

	// Get Camera Resolution ListBox
	CListBox *pList = (CListBox*)CListBox::FromHandle(GetDlgItem(m_hWndDraw, IDC_LIST3));

	if (pList) {
		// Clear Camera Resolution ListBox
		pList->ResetContent();

		// CaptureBuilder Find Interface IID_IAMStreamConfig
		IAMStreamConfig *pConfig = NULL;
		hr = m_pCaptureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0, m_pSelectCaptureFilter, IID_IAMStreamConfig, (void**)&pConfig);
		RETURN_FAILD_HRESULT(hr);

		int iCount = 0, iSize = 0;
		hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

		if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
			// Add Available Resolution to Listbox
			for (int i = 0; i < iCount; i++)
			{
				VIDEO_STREAM_CONFIG_CAPS vscc;
				AM_MEDIA_TYPE *pMediaType = NULL;
				VIDEOINFOHEADER *pVhd = NULL;
				CString cStrResolution;

				// Get MediaType Info
				hr = pConfig->GetStreamCaps(i, &pMediaType, (BYTE*)&vscc);
				RETURN_FAILD_HRESULT(hr);

				// if Video Type
				if (pMediaType->majortype == MEDIATYPE_Video &&
					pMediaType->formattype == FORMAT_VideoInfo) {
					// type cast to VIDEOINFOHEADER
					pVhd = reinterpret_cast<VIDEOINFOHEADER*> (pMediaType->pbFormat);

					// Add Resolution String to ListBox
					cStrResolution.Format(L"%d x %d / %d", pVhd->bmiHeader.biWidth, pVhd->bmiHeader.biHeight, i);
					pList->AddString(cStrResolution);
				}
				// Free MediaType
				DeleteMediaType(pMediaType);
			}
		}

		SAFE_RELEASE(pConfig);
	}

	return hr;
}

HRESULT CDShow::SelectCaptureFilter(CString deviceFriendlyName)
{
	HRESULT hr = S_OK;

	// 이름에 맞는 필터 서칭
	if (m_mapCamDevicesFilter.count(deviceFriendlyName) &&
		(m_mapCamDevicesFilter.size() > 0)) {
		m_pSelectCaptureFilter = m_mapCamDevicesFilter.find(deviceFriendlyName)->second;
	}
	else if ((deviceFriendlyName == L"") && (m_mapCamDevicesFilter.size() > 0)) {
		m_pSelectCaptureFilter = m_mapCamDevicesFilter.begin()->second;
	}
	else
		return E_FAIL;

	return hr;
}

HRESULT CDShow::SetResolution(CString resolution)
{
	HRESULT hr = S_OK;
	
	// Get Camera Resolution ListBox
	CListBox *pList = (CListBox*)CListBox::FromHandle(GetDlgItem(m_hWndDraw, IDC_LIST3));

	// CaptureBuilder Find Interface IID_IAMStreamConfig
	IAMStreamConfig *pConfig = NULL;
	hr = m_pCaptureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0, m_pSelectCaptureFilter, IID_IAMStreamConfig, (void**)&pConfig);
	RETURN_FAILD_HRESULT(hr);

	int iCount = 0, iSize = 0;
	hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
		// Add Available Resolution to Listbox
		for (int i = 0; i < iCount; i++)
		{
			VIDEO_STREAM_CONFIG_CAPS vscc;
			AM_MEDIA_TYPE *pMediaType = NULL;
			VIDEOINFOHEADER *pVhd = NULL;
			CString cStrResolution;
			bool bSetRes = false;

			// Get MediaType Info
			hr = pConfig->GetStreamCaps(i, &pMediaType, (BYTE*)&vscc);
			RETURN_FAILD_HRESULT(hr);

			// type cast to VIDEOINFOHEADER
			pVhd = reinterpret_cast<VIDEOINFOHEADER*> (pMediaType->pbFormat);

			cStrResolution.Format(L"%d x %d / %d", pVhd->bmiHeader.biWidth, pVhd->bmiHeader.biHeight, i);
			
			// if Video Type
			if (pMediaType->majortype == MEDIATYPE_Video &&
				pMediaType->formattype == FORMAT_VideoInfo)
			{
				// If Select Resolution Equal MediaType Resolution
				if (0 == StrCmpW(resolution, cStrResolution))
				{
					// Set MedaiType
					hr = pConfig->SetFormat(pMediaType);

					wprintf(L"Set Resolution: %s\n", resolution);
					bSetRes = true;
				}
				// Set Default
				else if ((resolution == L"") && (i == 0)) 
				{
					// Set MedaiType
					hr = pConfig->SetFormat(pMediaType);
					
					// If Set Default Resolution Auto Selecting Listbox
					pList->SelectString(-1, cStrResolution);

					wprintf(L"Set Resolution: Default (%s)\n", cStrResolution);
					bSetRes = true;
				}
			}
			
			// Free MediaType
			DeleteMediaType(pMediaType);

			if (bSetRes)
			{
				m_CurrentRes.x = pVhd->bmiHeader.biWidth;
				m_CurrentRes.y = pVhd->bmiHeader.biHeight;

				wprintf(L"m_CurrentRes: %d x %d\n", m_CurrentRes.x, m_CurrentRes.y);

				RETURN_FAILD_HRESULT(hr);
				break;
			}
		}
	}

	SAFE_RELEASE(pConfig);
	

	return hr;
}

HRESULT CDShow::SetScale(float scale)
{
	m_scale = scale;
	return S_OK;
}

void CDShow::SetSDKMode(DrawSDKMode sdkmode)
{
	m_pVideoRenderer->SetDrawSDKMode(sdkmode);
}

// ----- protected -----
HRESULT CDShow::InitCamDeviceList()
{
	HRESULT hr = S_OK;
	
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnumMoniker = NULL;
	IMoniker *pMoniker = NULL;
	CListBox *cListBox;
	ULONG cFetched;

	cListBox = (CListBox*)CListBox::FromHandle(GetDlgItem(m_hWndDraw, IDC_LIST2));

	

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pDevEnum);
	RETURN_FAILD_HRESULT(hr);

	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
	RETURN_FAILD_HRESULT(hr);

	while (pEnumMoniker->Next(1, &pMoniker, &cFetched) == S_OK) {
		IPropertyBag *pPropBag;
		IBaseFilter *pFilter;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
		RETURN_FAILD_HRESULT(hr);
		
		VARIANT varName;
		VariantInit(&varName);

		hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		RETURN_FAILD_HRESULT(hr);
		
		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**) &pFilter);

		m_vecCamDevicesList.push_back(varName);
		m_mapCamDevicesFilter.insert({ varName.bstrVal, pFilter });
		cListBox->AddString(varName.bstrVal);
		
		VariantClear(&varName);
		SAFE_RELEASE(pPropBag);
		SAFE_RELEASE(pMoniker);
	}

	SAFE_RELEASE(pEnumMoniker);
	SAFE_RELEASE(pDevEnum);

	return hr;
}

// ----- private -----

HRESULT CDShow::CameraSetWindow(HWND hViewWindow)
{
	HRESULT hr = S_OK;

	CRect rect;
	CPoint viewLeftTopPoint;
	CWnd *staticWindow;
	staticWindow = CWnd::FromHandle(hViewWindow);
	staticWindow->GetClientRect(&rect);

	// Window Setting
	//hr = m_pVW->put_Owner((OAHWND)staticWindow->GetSafeHwnd());
	//hr = m_pVideoRenderer->SetVideoWindow(staticWindow->GetSafeHwnd());
	//hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

	// if (m_CurrentRes.x > rect.Width()) {
	// 	m_scale = 0.5;
	// }

	m_CurrentRes.x *= m_scale;
	m_CurrentRes.y *= m_scale;
	viewLeftTopPoint.x = rect.CenterPoint().x - (m_CurrentRes.x / 2);
	viewLeftTopPoint.y = rect.CenterPoint().y - (m_CurrentRes.y / 2);
	// hr = m_pVW->SetWindowPosition(viewLeftTopPoint.x, viewLeftTopPoint.y, m_CurrentRes.x, m_CurrentRes.y);

	wprintf(L"CameraSetWindow: %d x %d\n", m_CurrentRes.x, m_CurrentRes.y);
	
	return hr;
}

void CDShow::DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
	// FreeFormatBlockForMediaType
	if (pmt != NULL) {
		CoTaskMemFree((PVOID)pmt->pbFormat);
		pmt->cbFormat = 0;
		pmt->pbFormat = NULL;
	}
	if (pmt->pUnk != NULL) {
		pmt->pUnk->Release();
		pmt->pUnk = NULL;
	}
	
	if (pmt != NULL) {
		CoTaskMemFree(pmt);
	}
}
