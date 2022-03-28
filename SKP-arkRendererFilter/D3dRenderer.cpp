#include "pch.h"
#include "D3dRenderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

	
	// Load Current Folder hlsl
	CString cstrPath;
	TCHAR path[MAX_PATH] = { 0, };

	// Get Directory by Current Execute File
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	cstrPath.SetString(path);
	cstrPath.Append(L"\\shaders.hlsl");

	// wprintf(L"ShaderFile Path:  %s\n", cstrPath);

	CD3Drenderer::ShaderCompile(cstrPath.GetBuffer());


	return true;
}

bool CD3Drenderer::ShaderCompile(wchar_t* filename)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pVsBlob = NULL, *pPsBlob = NULL, *pErrorBlob = NULL;
	
	// Convert to D3DCompile2 (After Version)
	HRESULT hr = D3DCompileFromFile(
		filename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"vs_texture",
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
		"ps_texture",
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
		{"POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		/*
		{ "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

bool CD3Drenderer::LoadTexture(wchar_t* filename)
{
	DirectX::ScratchImage image;
	DirectX::LoadFromWICFile(filename, DirectX::WIC_FLAGS_NONE, nullptr, image);
	//DirectX::LoadFromWICMemory
	DirectX::CreateShaderResourceView(m_pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_pTexture);

	return true;
}

bool CD3Drenderer::Draw(const BYTE* pRgb32Buffer)
{
	// Draw Call MFCDirectShowTestApp.exe Test
	// static int count = 0;
	// wprintf(L"Draw Direct3D 11 \\ %d\n", count++);
	
	// Create Vertex Buffer
	float vertex_data_array[] = {
		0.0f, 0.5f, 0.0f, // top
		0.5f, -0.5f, 0.0f, // botoom-right
		-0.5f, -0.5f, 0.0f, // botoom-left
	};

	float TextureVertexData[] = { // x, y, u, v
			-0.5f,  0.5f, 0.f, 0.f,
			0.5f, -0.5f, 1.f, 1.f,
			-0.5f, -0.5f, 0.f, 1.f,
			-0.5f,  0.5f, 0.f, 0.f,
			0.5f,  0.5f, 1.f, 0.f,
			0.5f, -0.5f, 1.f, 1.f
	};

	UINT vertex_stride = 4 * sizeof(float);
	UINT vertex_count = sizeof(TextureVertexData) / vertex_stride;
	UINT vertex_offset = 0;

	ID3D11Buffer *pVertexBuffer = NULL;
	
	// Load Mesh Data into vertex buffer
	D3D11_BUFFER_DESC vertexBuffDesc = { 0, };
	vertexBuffDesc.ByteWidth = sizeof(TextureVertexData);
	vertexBuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA sr_data = { TextureVertexData };
	sr_data.pSysMem = TextureVertexData;
	HRESULT hr = m_pDevice->CreateBuffer(
		&vertexBuffDesc,
		&sr_data,
		&pVertexBuffer
	);
	assert(SUCCEEDED(hr));

	// Create Sampler State
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	ID3D11SamplerState* samplerState;
	m_pDevice->CreateSamplerState(&samplerDesc, &samplerState);

	// Load Image
	char filename[] = "testTexture.png";

	int texWidth, texHeight, texNumChannels;
	int texForceNumChannels = 4;
	unsigned char* fileBytes = stbi_load(filename, &texWidth, &texHeight, &texNumChannels, texForceNumChannels);
	//unsigned char* fileBytes = (unsigned char*)pRgb32Buffer;

	assert(fileBytes);
	int texBytesPerRow = 4 * texWidth;

	// Create Texture
	D3D11_TEXTURE2D_DESC textureDesc = { 0, };
	textureDesc.Width = texWidth;
	textureDesc.Height = texHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
	textureSubresourceData.pSysMem = pRgb32Buffer;
	textureSubresourceData.SysMemPitch = texBytesPerRow;

	ID3D11Texture2D* texture;
	m_pDevice->CreateTexture2D(&textureDesc, &textureSubresourceData, &texture);

	ID3D11ShaderResourceView* textureView;
	m_pDevice->CreateShaderResourceView(texture, nullptr, &textureView);

	free(fileBytes);
	


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

	m_pDeviceContext->PSSetShaderResources(0, 1, &textureView);
	m_pDeviceContext->PSSetSamplers(0, 1, &samplerState);

	m_pDeviceContext->Draw(vertex_count, 0);
	
	m_pSwapChain->Present( 1, 0 );

	return true;
}

bool CD3Drenderer::Test()
{
	


	return true;
}
