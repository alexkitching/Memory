#pragma once
#include "Renderer.h"

#include "D3D.h"
#include <wrl.h>

class D3DRenderer : public IRenderer
{
public:
	D3DRenderer(HWND a_hwnd);
	virtual ~D3DRenderer() {}

	D3DRenderer(const D3DRenderer&) = delete;
	D3DRenderer& operator=(const D3DRenderer&) = delete;

	virtual void EndFrame() override;
	virtual void Clear(float a_r, float a_g, float a_b) override;
private:

	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget;
};