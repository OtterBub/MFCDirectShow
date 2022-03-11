#include "pch.h"
#include "D3dRenderer.h"

CD3Drenderer::CD3Drenderer()
{
	m_pDevice = NULL;
	m_pDeviceContext = NULL;
	m_pSwapChain = NULL;
	m_pRenderTargetView = NULL;
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
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B5G6R5_UNORM;
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

	assert( S_OK == hr &&
	m_pSwapChain &&
	m_pDevice &&
	m_pDeviceContext);

	ID3D11Texture2D* framebuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**)&framebuffer);
	assert(SUCCEEDED(hr));

	hr = m_pDevice->CreateRenderTargetView(framebuffer, 0, &m_pRenderTargetView);
	assert(SUCCEEDED(hr));
	framebuffer->Release();

	CD3Drenderer::ShaderCompile(TEXT("shaders.hlsl"));

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

	ID3D11VertexShader *pVertexShader = NULL;
	ID3D11PixelShader *pPixelShader = NULL;

	hr = m_pDevice->CreateVertexShader(
		pVsBlob->GetBufferPointer(),
		pVsBlob->GetBufferSize(),
		NULL,
		&pVertexShader
	);
	assert(SUCCEEDED(hr));

	hr = m_pDevice->CreatePixelShader(
		pPsBlob->GetBufferPointer(),
		pPsBlob->GetBufferSize(),
		NULL,
		&pPixelShader
	);
	assert(SUCCEEDED(hr));

	ID3D11InputLayout* pInputLayout = NULL;
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
		&pInputLayout
	);
	assert(SUCCEEDED(hr));

	return true;
}

bool CD3Drenderer::Draw()
{
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

	// Draw Do~~
	return true;
}

bool CD3Drenderer::Test()
{
	


	return true;
}
