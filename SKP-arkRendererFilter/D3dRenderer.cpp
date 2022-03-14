#include "pch.h"
#include "D3dRenderer.h"

CD3Drenderer::CD3Drenderer()
{
	m_pDevice = NULL;
	m_pDeviceContext = NULL;
	m_pSwapChain = NULL;
	m_pRenderTargetView = NULL;
	m_pInputLayout = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
}

CD3Drenderer::~CD3Drenderer()
{
	
}

bool CD3Drenderer::init(HWND hwnd)
{
	m_hWnd = hwnd;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0, };
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.Windowed = true;

	D3D_FEATURE_LEVEL featureLevel;
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_pSwapChain,
		&m_pDevice,
		&featureLevel,
		&m_pDeviceContext
	);
	assert(m_hWnd);
	assert(m_pSwapChain);
	assert(m_pDevice);
	assert(m_pDeviceContext);
	assert(S_OK == hr);

	ID3D11Texture2D* framebuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**)&framebuffer);
	assert(SUCCEEDED(hr));

	hr = m_pDevice->CreateRenderTargetView(framebuffer, 0, &m_pRenderTargetView);
	assert(SUCCEEDED(hr));
	framebuffer->Release();

	CD3Drenderer::ShaderCompile(TEXT("C:\\Users\\skpark\\Documents\\WorkingForder\\TestProject\\MFC_Test\\MFCDirectShow\\SKP-arkRendererFilter\\shaders.hlsl"));

	return true;
}

bool CD3Drenderer::ShaderCompile(wchar_t* filename)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pVsBlob = NULL, *pPsBlob = NULL, *pErrorBlob = NULL;

	HRESULT hr = D3DCompileFromFile(
		filename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"vs_main",
		"vs_5_0",
		flags,
		0,
		&pVsBlob,
		&pErrorBlob
	);
	if (FAILED(hr)) {
		if (pErrorBlob) {
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		if (pVsBlob) {
			pVsBlob->Release();
		}
		assert(false);
	}

	hr = D3DCompileFromFile(
		filename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"ps_main",
		"ps_5_0",
		flags,
		0,
		&pPsBlob,
		&pErrorBlob
	);
	if (FAILED(hr)) {
		if (pErrorBlob) {
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		if (pPsBlob) {
			pPsBlob->Release();
		}
		assert(false);
	}

	hr = m_pDevice->CreateVertexShader(
		pVsBlob->GetBufferPointer(),
		pVsBlob->GetBufferSize(),
		NULL,
		&m_pVertexShader
	);
	assert(SUCCEEDED(hr));

	hr = m_pDevice->CreatePixelShader(
		pPsBlob->GetBufferPointer(),
		pPsBlob->GetBufferSize(),
		NULL,
		&m_pPixelShader
	);
	assert(SUCCEEDED(hr));

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		/*
		{ "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		*/
	};

	hr = m_pDevice->CreateInputLayout(
		inputElementDesc,
		ARRAYSIZE(inputElementDesc),
		pVsBlob->GetBufferPointer(),
		pVsBlob->GetBufferSize(),
		&m_pInputLayout
	);
	assert(SUCCEEDED(hr));

	return true;
}

bool CD3Drenderer::Draw()
{
	static int count = 0;
	wprintf(L"Draw Direct3D 11 \\ %d\n", count++);
	// Create Vertex Buffer

	float vertex_data_array[] = {
		0.0f, 0.5f, 0.0f, // top
		0.5f, -0.5f, 0.0f, // botoom-right
		-0.5f, -0.5f, 0.0f, // botoom-left
	};
	UINT vertex_stride = 3 * sizeof(float);
	UINT vertex_offset = 0;
	UINT vertex_count = 3;

	ID3D11Buffer *pVertexBuffer = NULL;
	
	// Load Mesh Data into vertex buffer
	D3D11_BUFFER_DESC vertexBuffDesc = { 0, };
	vertexBuffDesc.ByteWidth = sizeof(vertex_data_array);
	vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA sr_data = { 0, };
	sr_data.pSysMem = vertex_data_array;
	HRESULT hr = m_pDevice->CreateBuffer(
		&vertexBuffDesc,
		&sr_data,
		&pVertexBuffer
	);
	assert(SUCCEEDED(hr));


	// Clear Back Buffer
	float background_color[4] = {
		0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f
	};

	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, background_color);

	RECT winRect;
	GetClientRect(m_hWnd, &winRect);
	D3D11_VIEWPORT viewport = {
		0.0f,
		0.0f,
		(FLOAT)(winRect.right - winRect.left),
		(FLOAT)(winRect.bottom - winRect.top),
		0.0f,
		1.0f
	};

	m_pDeviceContext->RSSetViewports(1, &viewport);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	m_pDeviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->IASetVertexBuffers(
		0,
		1,
		&pVertexBuffer,
		&vertex_stride,
		&vertex_offset
	);
	m_pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	m_pDeviceContext->Draw(vertex_count, 0);

	m_pSwapChain->Present( 1, 0 );

	// Draw Do~~
	return true;
}

bool CD3Drenderer::Test()
{
	


	return true;
}
