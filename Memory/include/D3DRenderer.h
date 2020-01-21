#pragma once
#include "Renderer.h"

#include "D3D.h"
#include <wrl.h>

//------------
// Description
//--------------
// DirectX Renderer Class, Provides Present/Clear DirectX Functionality with a basic Draw Cube Function
//------------

struct Vertex
{
	struct
	{
		float x;
		float y;
		float z;
	}Pos;
};

struct Cube
{
	static inline const Vertex s_kVertices[] =
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

	static inline const unsigned short s_kIndices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	static Microsoft::WRL::ComPtr<ID3D11Buffer> s_pVertexBuffer;
	static Microsoft::WRL::ComPtr<ID3D11Buffer> s_pIndexBuffer;
};

class D3DRenderer : public IRenderer
{
public:
	D3DRenderer(HWND a_hwnd);
	virtual ~D3DRenderer() {}

	D3DRenderer(const D3DRenderer&) = delete;
	D3DRenderer& operator=(const D3DRenderer&) = delete;

	virtual void EndFrame() override;
	virtual void Clear(float a_r, float a_g, float a_b) override;

	void CreateShaders();
	void CreateCubeBuffers();
	void DrawCube(float a_x, float a_y, float a_z, float a_scale, float a_angle) override;

	ID3D11Device* GetDevice() const { return m_pDevice.Get(); }
	IDXGISwapChain* GetSwapChain() const { return m_pSwapChain.Get(); }
	ID3D11DeviceContext* GetContext() const { return m_pContext.Get(); }
	
private:

	// D3D Com Objects
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
	
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	// Input (Vertex) Layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pVertexInputLayout;
};
