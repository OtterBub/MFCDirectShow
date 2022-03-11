#include "pch.h"
#include "SKP-arkRendererFilter_Guids.h""
#include "SKP-arkRendererFilter.h"

// Setup data

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
	&MEDIATYPE_Video,            // Major type
	&MEDIASUBTYPE_NULL          // Minor type
};

const AMOVIESETUP_PIN sudOpPin =
{
	L"Input",                   // Pin string name
	TRUE,                      // Is it rendered
	FALSE,                      // Is it an output
	FALSE,                      // Allowed none
	FALSE,                      // Likewise many
	&CLSID_NULL,                // Connects to filter
	L"Output",                  // Connects to pin
	1,                          // Number of types
	&sudOpPinTypes                // Pin information
};

const AMOVIESETUP_FILTER sudOpFilter =
{
	&CLSID_SKParkRendererFilter,                // Filter CLSID
	FILTER_NAME,                    // String name
	MERIT_DO_NOT_USE,           // Filter merit
	1,                          // Number pins
	&sudOpPin                    // Pin details
};


//
//  Object creation stuff
//
CFactoryTemplate g_Templates[] = {
	FILTER_NAME, &CLSID_SKParkRendererFilter, CSKParkRender::CreateInstance, NULL, &sudOpFilter
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

//
// DllRegisterSever
//
// Handle the registration of this filter
//
STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);

} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);

} // DllUnregisterServer


//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  dwReason,
	LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

