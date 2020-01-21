#include "D3DRenderer.h"
#include "Profiler.h"
#include <d3dcompiler.h>


Microsoft::WRL::ComPtr<ID3D11Buffer> Cube::s_pVertexBuffer = nullptr;
Microsoft::WRL::ComPtr<ID3D11Buffer> Cube::s_pIndexBuffer = nullptr;


D3DRenderer::D3DRenderer(HWND a_hwnd)
	:
m_pDevice(nullptr),
m_pSwapChain(nullptr),
m_pContext(nullptr),
m_pRenderTarget(nullptr)
{
	// Define Swap Chain
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

	
	CreateShaders();
	CreateCubeBuffers();
}

void D3DRenderer::EndFrame()
{
	PROFILER_BEGIN_SAMPLE(D3DRenderer::EndFrame());
	HRESULT hr = m_pSwapChain->Present(1u, 0u); // 1u - 60fps 2u - 30fps

	if(FAILED(hr)) // Present Failed
	{
		if(hr == DXGI_ERROR_DEVICE_REMOVED) // Device Remove - Get Actual Reason
		{
			D3D_THROW_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		}
		else
		{
			D3D_THROW_FAILED(hr);
		}
	}
	
	PROFILER_END_SAMPLE();
}

void D3DRenderer::Clear(float a_r, float a_g, float a_b)
{
	// Clear Render Targets
	const float color[] = { a_r, a_g, a_b, 1.f };
	m_pContext->ClearRenderTargetView(m_pRenderTarget.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

void D3DRenderer::CreateShaders()
{
#if DEBUG
	HRESULT hr;
#endif
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

	// Create Pixel Shader
	D3D_THROW_FAILED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	D3D_THROW_FAILED(m_pDevice->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_pPixelShader));

	// Create Vertex Shader
	D3D_THROW_FAILED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	D3D_THROW_FAILED(m_pDevice->CreateVertexShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_pVertexShader));

	// Create Input Layout for Vertex Shader
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3D_THROW_FAILED(m_pDevice->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&m_pVertexInputLayout));
	
}

void D3DRenderer::CreateCubeBuffers()
{
	// Create Vertex Buffer
	D3D11_BUFFER_DESC bd =
	{
		sizeof(Cube::s_kVertices),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0u,
		0u,
		sizeof(Vertex)
	};

	D3D11_SUBRESOURCE_DATA srd =
	{
		Cube::s_kVertices,
		0u,
		0u
	};

	// Create Buffer
#if DEBUG
	HRESULT hr;
#endif
	D3D_THROW_FAILED(m_pDevice->CreateBuffer(&bd, &srd, &Cube::s_pVertexBuffer));

	// Create Index Buffer
	D3D11_BUFFER_DESC idb =
	{
		sizeof(Cube::s_kIndices),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_INDEX_BUFFER,
		0u,
		0u,
		sizeof(unsigned short)
	};

	D3D11_SUBRESOURCE_DATA isd =
	{
		Cube::s_kIndices,
		0u,
		0u
	};

	D3D_THROW_FAILED(m_pDevice->CreateBuffer(&idb, &isd, &Cube::s_pIndexBuffer));
}


void D3DRenderer::DrawCube(float a_x, float a_y, float a_z, float a_scale, float a_angle)
{
	PROFILER_BEGIN_SAMPLE(D3DRenderer::DrawCube);

#if DEBUG
	HRESULT hr;
#endif

	PROFILER_BEGIN_SAMPLE(D3DRenderer::SetVertexBuffers);
	// Bind Vertex Buffer to Pipeline
	UINT stride = sizeof(Vertex);
	UINT offset = 0u;
	D3D_THROW_INFO(m_pContext->IASetVertexBuffers(0u, 1u, Cube::s_pVertexBuffer.GetAddressOf(), &stride, &offset));
	PROFILER_END_SAMPLE();
	PROFILER_BEGIN_SAMPLE(D3DRenderer::SetIndexBuffer);
	// Bind Index Buffer to Pipeline
	D3D_THROW_INFO(m_pContext->IASetIndexBuffer(Cube::s_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
	PROFILER_END_SAMPLE();

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
	PROFILER_BEGIN_SAMPLE(D3DRenderer::VSCreate/SetConstantBuffers);
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

	PROFILER_END_SAMPLE();

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

	PROFILER_BEGIN_SAMPLE(D3DRenderer::PSCreate / SetConstantBuffers);

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

	PROFILER_END_SAMPLE();


	PROFILER_BEGIN_SAMPLE(D3DRenderer::BindShaders);
	// Bind Pixel Shader
	D3D_THROW_INFO(m_pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0u));

	// Bind Vertex Shader
	D3D_THROW_INFO(m_pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0u));

	PROFILER_END_SAMPLE();

	PROFILER_BEGIN_SAMPLE(D3DRenderer::IASetLayout);
	D3D_THROW_INFO(m_pContext->IASetInputLayout(m_pVertexInputLayout.Get()));
	
	// Set Primitive Topology to Triangle List
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	PROFILER_END_SAMPLE();

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

	PROFILER_BEGIN_SAMPLE(D3DRenderer::DrawIndexed);

	D3D_THROW_INFO(m_pContext->DrawIndexed((UINT)std::size(Cube::s_kIndices), 0u, 0u));
	PROFILER_END_SAMPLE();
	
	PROFILER_END_SAMPLE();
}
