#include "D3DRenderer.h"


D3DRenderer::D3DRenderer(HWND a_hwnd)
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
	sd.OutputWindow = (HWND)696969;//a_hwnd; //a_hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr;


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
	ID3D11Resource* pBackbuffer = nullptr;
	D3D_THROW_FAILED(m_pSwapChain->GetBuffer(0, // Buffer Idx
		__uuidof(ID3D11Resource),
		reinterpret_cast<void**>(&pBackbuffer)));

	D3D_THROW_FAILED(m_pDevice->CreateRenderTargetView(
		pBackbuffer,
		nullptr,
		&m_pRenderTarget));

	pBackbuffer->Release();
}

D3DRenderer::~D3DRenderer()
{	
	if(m_pRenderTarget != nullptr)
	{
		m_pRenderTarget->Release();
	}
	
	if(m_pContext != nullptr)
	{
		m_pContext->Release();
	}
	
	if (m_pSwapChain != nullptr)
	{
		m_pSwapChain->Release();
	}
	
	if(m_pDevice != nullptr)
	{
		m_pDevice->Release();
	}
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
	m_pContext->ClearRenderTargetView(m_pRenderTarget, color);
}