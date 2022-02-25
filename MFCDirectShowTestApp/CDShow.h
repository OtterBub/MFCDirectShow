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
	if (FAILED(x))\
		return x;\
}

class CDShow 
{
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
	HRESULT CameraStart();
	HRESULT CameraStart(CString deviceFriendlyName);

	// Camera Device Capture Stop
	HRESULT CameraStop();


protected:
	HRESULT InitCamDeviceList();

private:
	

	// Setting VW By WindowHandle
	HRESULT CameraSetWindow(HWND hViewWindow);

	// For Draw Window
	HWND m_hWndDraw;
	
	IMediaControl *m_pMC;
	IVideoWindow *m_pVW;
	IMediaEventEx *m_pME;
	
	// ----- For Capture Camera Device -----
	IGraphBuilder *m_pGraph;
	ICaptureGraphBuilder2 *m_pCaptureBuilder;
	IBaseFilter *m_pVideoCapFilter;
	IBaseFilter *m_pCurrentCaptureFilter;

	// CamDevicesList (Friendly Name)
	std::vector<VARIANT> m_vecCamDevicesList;
	// CamDevices Filter (key is Friendly Name)
	std::map<CString, IBaseFilter*> m_mapCamDevicesFilter;

};