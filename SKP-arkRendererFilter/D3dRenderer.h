#pragma once

// Direct 3D Class

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>


class CD3Drenderer
{
public:
	CD3Drenderer();
	~CD3Drenderer();

	bool init(HWND hwnd);
	bool ShaderCompile(wchar_t* filename);
	
	bool Draw();

	bool Test();

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;

	HWND m_hWnd;

protected:

private:

};