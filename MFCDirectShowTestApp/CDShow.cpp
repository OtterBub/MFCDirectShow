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
}

CDShow::~CDShow() 
{
	SAFE_RELEASE(m_pGraph);
	SAFE_RELEASE(m_pCaptureBuilder);
	SAFE_RELEASE(m_pMC);
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

	hr = m_pCaptureBuilder->SetFiltergraph(m_pGraph);
	RETURN_FAILD_HRESULT(hr);

	hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pMC);
	RETURN_FAILD_HRESULT(hr);
	
	hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&m_pVW);
	RETURN_FAILD_HRESULT(hr);
	
	hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_pME);
	RETURN_FAILD_HRESULT(hr);

	InitCamDeviceList();

	return hr;
}

HRESULT CDShow::Initialize(HWND hwnd)
{
	HRESULT hr = S_OK;
	RECT wrc;
	GetWindowRect(hwnd, &wrc);

	m_hWndDraw = hwnd;
	
	hr = CDShow::Initialize();
	RETURN_FAILD_HRESULT(hr);

	return hr;
}

HRESULT CDShow::UnInitialize()
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_pGraph);
	SAFE_RELEASE(m_pCaptureBuilder);
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW);
	SAFE_RELEASE(m_pME);

	for each (std::pair<CString, IBaseFilter*> var in m_mapCamDevicesFilter) {
		SAFE_RELEASE(var.second);
	}

	CoUninitialize();

	return hr;
}

std::vector<VARIANT> CDShow::GetCamDeviceList() 
{
	return m_vecCamDevicesList;
}

HRESULT CDShow::CameraStart()
{
	HRESULT hr = S_OK;

	if (m_mapCamDevicesFilter.size() > 0)
		m_pCurrentCaptureFilter = m_mapCamDevicesFilter.begin()->second;
	else
		return S_FALSE;

	hr = m_pGraph->AddFilter(m_pCurrentCaptureFilter, NULL);
	RETURN_FAILD_HRESULT(hr);

	hr = m_pCaptureBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pCurrentCaptureFilter, NULL, NULL);
	RETURN_FAILD_HRESULT(hr);

	CameraSetWindow(GetDlgItem(m_hWndDraw, IDC_STATIC3));
	
	m_pMC->Run();

	return hr;
}

HRESULT CDShow::CameraStart(CString deviceFriendlyName)
{
	HRESULT hr = S_OK;

	// 이름에 맞는 필터 서칭
	if (m_mapCamDevicesFilter.count(deviceFriendlyName) &&
		(m_mapCamDevicesFilter.size() > 0))
		m_pCurrentCaptureFilter = m_mapCamDevicesFilter.find(deviceFriendlyName)->second;
	else
		return S_FALSE;

	hr = m_pGraph->AddFilter(m_pCurrentCaptureFilter, NULL);
	RETURN_FAILD_HRESULT(hr);

	hr = m_pCaptureBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pCurrentCaptureFilter, NULL, NULL);
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
	CWnd *staticWindow;
	staticWindow = CWnd::FromHandle(hViewWindow);
	staticWindow->GetClientRect(&rect);

	// Window Setting
	hr = m_pVW->put_Owner((OAHWND)staticWindow->GetSafeHwnd());
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = m_pVW->SetWindowPosition(0, 0, rect.Width(), rect.Height());
	staticWindow->GetWindowRect(&rect);
	
	return hr;
}
