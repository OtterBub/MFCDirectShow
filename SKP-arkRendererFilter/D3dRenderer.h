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
	bool LoadTexture(wchar_t* filename);
	bool CreateVertexData();
	
	bool Draw(const BYTE* pRgb32Buffer);

	bool Test();

	
protected:

private:
	bool CreateTextureBuffer(const BYTE* pRgb32Buffer);

	bool m_firstCreateBuffer;
	float* m_TextureVertexData;
	std::vector<float> m_vecTextureVertexData;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11InputLayout* m_pInputLayout;

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	ID3D11ShaderResourceView* m_pTexture;


	ID3D11SamplerState* m_pSamplerState;
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Texture2D* m_pTexture2D;

	const BYTE* m_pCamByte;

	HWND m_hWnd;

};