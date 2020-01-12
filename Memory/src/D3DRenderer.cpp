#include "D3DRenderer.h"

D3DRenderer::D3DRenderer(HWND a_hwnd)
	:
m_pDevice(nullptr),
m_pSwapChain(nullptr),
m_pContext(nullptr),
m_pRenderTarget(nullptr)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // AA
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1; // 1 Back Buffer
	sd.OutputWindow = a_hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

#if DEBUG
	HRESULT hr;
#endif

	UINT flags = 0;
#if DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create Device and front/back buffers, and swap chain and rendering context
	D3D_THROW_FAILED( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwapChain,
		&m_pDevice,
		nullptr,
		&m_pContext
	));

	// Gain Access to texture subresource in swap chain (backbuffer)
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackbuffer = nullptr;
	D3D_THROW_FAILED(m_pSwapChain->GetBuffer(0, // Buffer Idx
		__uuidof(ID3D11Resource),
		&pBackbuffer));

	D3D_THROW_FAILED(m_pDevice->CreateRenderTargetView(
		pBackbuffer.Get(),
		nullptr,
		&m_pRenderTarget));

	// Create Depth Stencil State
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	
	D3D_THROW_FAILED(m_pDevice->CreateDepthStencilState(&dsDesc, &pDepthStencilState));

	// Bind to Output Merger
	D3D_THROW_INFO(m_pContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1u));

	// Create Depth Stencil Texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilTex;
	D3D11_TEXTURE2D_DESC dsTexDesc = {};
	dsTexDesc.Width = 1280u;
	dsTexDesc.Height = 768u;
	dsTexDesc.MipLevels = 1u;
	dsTexDesc.ArraySize = 1u;
	dsTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsTexDesc.SampleDesc.Count = 1u;
	dsTexDesc.SampleDesc.Quality = 0u;
	dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	D3D_THROW_FAILED(m_pDevice->CreateTexture2D(&dsTexDesc, nullptr, &pDepthStencilTex));

	// Create view of Depth Stencil Texture
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc = {};
	DSVdesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVdesc.Texture2D.MipSlice = 0u;

	D3D_THROW_FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTex.Get(), &DSVdesc, &m_pDepthStencilView));

	// Bind Depth Stencil View to Output Merger
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(1u, m_pRenderTarget.GetAddressOf(), m_pDepthStencilView.Get()));
	
}

void D3DRenderer::EndFrame()
{
	HRESULT hr = m_pSwapChain->Present(1u, 0u); // 1u - 60fps 2u - 30fps

	if(FAILED(hr))
	{
		if(hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			D3D_THROW_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		}
		else
		{
			D3D_THROW_FAILED(hr);
		}
	}
	
	
}

void D3DRenderer::Clear(float a_r, float a_g, float a_b)
{
	const float color[] = { a_r, a_g, a_b, 1.f };
	m_pContext->ClearRenderTargetView(m_pRenderTarget.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

void D3DRenderer::DrawCube(float a_x, float a_y, float a_z, float a_scale, float a_angle)
{
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		}Pos;
	};

	// Create Vertex Buffer for 1D Triangle
	Vertex vertices[] =
	{
		{-1.f, -1.f, -1.f},
		{1.f, -1.f, -1.f},
		{-1.f, 1.f, -1.f},
		{1.f, 1.f, -1.f},
		{-1.f, -1.f, 1.f},
		{1.f, -1.f, 1.f},
		{-1.f, 1.f, 1.f},
		{1.f, 1.f, 1.f},
	};



	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd =
	{
		sizeof(vertices),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0u,
		0u,
		sizeof(Vertex)
	};

	D3D11_SUBRESOURCE_DATA srd =
	{
		vertices,
		0u,
		0u
	};

	// Create Buffer
#if DEBUG
	HRESULT hr;
#endif
	D3D_THROW_FAILED(m_pDevice->CreateBuffer(&bd, &srd, &pVertexBuffer));


	// Bind Vertex Buffer to Pipeline
	UINT stride = sizeof(Vertex);
	UINT offset = 0u;
	D3D_THROW_INFO(m_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));

	// Create Index Buffer
	const unsigned short indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC idb =
	{
		sizeof(indices),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_INDEX_BUFFER,
		0u,
		0u,
		sizeof(unsigned short)
	};

	D3D11_SUBRESOURCE_DATA isd =
	{
		indices,
		0u,
		0u
	};

	D3D_THROW_FAILED(m_pDevice->CreateBuffer(&idb, &isd, &pIndexBuffer));

	// Bind Index Buffer to Pipeline
	D3D_THROW_INFO(m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));


	// Constant Buffer for Transformation Matrix
	struct VSConstantBuffer
	{
		DX::XMMATRIX transform;
	};

	const float fCamZOffs = 10;
	// Buffer Data Instance
	const VSConstantBuffer vscb =
	{
		{
				DX::XMMatrixTranspose(
					DX::XMMatrixScaling(a_scale, a_scale, a_scale) *
					DX::XMMatrixRotationZ(a_angle) *
					DX::XMMatrixRotationX(a_angle) *
					DX::XMMatrixTranslation(a_x, a_y, a_z  + fCamZOffs) *
					DX::XMMatrixPerspectiveLH(1.f, 3.f / 4.f, 0.5f, 500.f)
				)
		}
	};

	// Create Buffer

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVSConstantBuffer;
	D3D11_BUFFER_DESC vscbd =
	{
		sizeof(vscb),
		D3D11_USAGE_DYNAMIC, // Dynamic Allows use of Lock Function
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0u,
		0u // Not an array
	};

	D3D11_SUBRESOURCE_DATA vscsd =
	{
		&vscb,
		0u,
		0u
	};

	D3D_THROW_FAILED(m_pDevice->CreateBuffer(&vscbd, &vscsd, &pVSConstantBuffer));

	// Bind Buffer to Vertex Shader
	D3D_THROW_INFO(m_pContext->VSSetConstantBuffers(0u, 1u, pVSConstantBuffer.GetAddressOf()));

	struct PSConstantBuffer
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} Face_Colours[6];
	};

	PSConstantBuffer pscb =
	{
		{
			{1.f, 0.f, 1.f},
			{1.f, 0.f, 0.f},
			{0.f, 1.f, 0.f},
			{0.f, 0.f, 1.f},
			{1.f, 1.f, 0.f},
			{0.f, 1.f, 1.f}
		}
	};

	// Create Buffer

	Microsoft::WRL::ComPtr<ID3D11Buffer> pPSConstantBuffer;
	D3D11_BUFFER_DESC pscbd =
	{
		sizeof(pscb),
		D3D11_USAGE_DYNAMIC, // Dynamic Allows use of Lock Function
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0u,
		0u // Not an array
	};

	D3D11_SUBRESOURCE_DATA pscsd =
	{
		&pscb,
		0u,
		0u
	};

	D3D_THROW_FAILED(m_pDevice->CreateBuffer(&pscbd, &pscsd, &pPSConstantBuffer));

	D3D_THROW_INFO(m_pContext->PSSetConstantBuffers(0u, 1u, pPSConstantBuffer.GetAddressOf()));

	// Create Pixel Shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3D_THROW_FAILED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	D3D_THROW_FAILED(m_pDevice->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&pPixelShader));

	// Bind Pixel Shader
	D3D_THROW_INFO(m_pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u));

	// Create Vertex Shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	D3D_THROW_FAILED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	D3D_THROW_FAILED(m_pDevice->CreateVertexShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&pVertexShader));

	// Bind Vertex Shader
	D3D_THROW_INFO(m_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u));

	// Input (Vertex) Layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3D_THROW_FAILED(m_pDevice->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout));

	D3D_THROW_INFO(m_pContext->IASetInputLayout(pInputLayout.Get()));
	
	// Set Primitive Topology to Triangle List
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Config Viewport
	D3D11_VIEWPORT vp =
	{
		0,
		0,
		1280,
		768,
		0,
		1
	};

	// Set Viewports
	D3D_THROW_INFO(m_pContext->RSSetViewports(1u, &vp));

	D3D_THROW_INFO(m_pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
}
