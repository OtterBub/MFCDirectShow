#pragma once

#include <dshow.h>
#include <vector>
#include <string>
#include <map>
#include "Resource.h"

#define SAFE_RELEASE(x) \
{ \
	if (x != NULL) \
		x->Release(); \
		x = NULL; \
}

#define RETURN_FAILD_HRESULT(x) \
{\
	if (FAILED(x)) {\
		TRACE(L"FAILED(x) = 0x%x", x);\
		return x;\
	}\
}


class CDShow 
{
// ----- Function -----
public:
	CDShow();
	~CDShow();

	// DirectShow Initialize
	HRESULT Initialize();
	HRESULT Initialize(HWND hwnd);
	
	// DirectShow UnInitialize
	HRESULT UnInitialize();

	// Get Camera Devices Friendly Name List
	std::vector<VARIANT> GetCamDeviceList();

	// Camera Device Capture Start
	HRESULT CameraStart(CString deviceFriendlyName = CString(L""));

	// Camera Device Capture Stop
	HRESULT CameraStop();

	// Refresh Resolution List by Current Camera Device
	HRESULT AddCamDeviceResolutionList();

	// Set SelectFilter by deviceFriendly Name
	HRESULT SelectCaptureFilter(CString deviceFriendlyName = CString(L""));

	// Set Resolution By "000 x 000 / 0"
	HRESULT SetResolution(CString resolution = CString(L""));

	// Set Resolution Scale
	HRESULT SetScale(float scale);

protected:
	HRESULT InitCamDeviceList();

private:
	// Setting VW By WindowHandle
	HRESULT CameraSetWindow(HWND hViewWindow);
	void DeleteMediaType(AM_MEDIA_TYPE *pmt);


// ----- Member -----
public:

protected:

private:

	// For Draw Window
	HWND m_hWndDraw;
	
	IMediaControl *m_pMC;
	IVideoWindow *m_pVW;
	IMediaEventEx *m_pME;

	CPoint m_CurrentRes;
	float m_scale;
	
	// ----- For Capture Camera Device -----
	IGraphBuilder *m_pGraph;
	ICaptureGraphBuilder2 *m_pCaptureBuilder;
	IBaseFilter *m_pVideoCapFilter;

	// Current Capture Filter
	IBaseFilter *m_pCurrentCaptureFilter;

	// Select Capture Filter on ListBox
	IBaseFilter *m_pSelectCaptureFilter;

	// CamDevicesList (Friendly Name)
	std::vector<VARIANT> m_vecCamDevicesList;

	// CamDevices Filter (key is Friendly Name)
	std::map<CString, IBaseFilter*> m_mapCamDevicesFilter;

};