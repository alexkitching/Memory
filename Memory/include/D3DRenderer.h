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

	void DrawTestTriangle()
	{
		struct Vertex
		{
			struct
			{
				float x;
				float y;
			}Pos;

			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			}Col;
		};

		// Create Vertex Buffer for 1D Triangle
		Vertex vertices[] =
		{
			{0.f, 0.5f, 255, 0, 0, 0},
			{0.5f, -0.5f, 0, 255, 0, 0},
			{-0.5f, -0.5f, 0, 0, 255, 0},
			{-0.3f, 0.3f, 0, 255, 0, 0},
			{0.3f, 0.3f, 0, 0, 255, 0},
			{0.f, -1.8f, 255, 0, 0, 0}
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
		HRESULT hr;
		D3D_THROW_FAILED(m_pDevice->CreateBuffer(&bd, &srd, &pVertexBuffer));
	

		// Bind Vertex Buffer to Pipeline
		UINT stride = sizeof(Vertex);
		UINT offset = 0u;
		D3D_THROW_INFO(m_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset ));

		// Create Index Buffer
		const unsigned short indices[] =
		{
			0, 1, 2,
			0, 2, 3,
			0, 4, 1,
			2, 1, 5
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

		D3D_THROW_INFO(m_pDevice->CreateBuffer(&idb, &isd, &pIndexBuffer));

		// Bind Index Buffer to Pipeline
		D3D_THROW_INFO(m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
		
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
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOUR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		
		D3D_THROW_FAILED(m_pDevice->CreateInputLayout(
			ied,
			(UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout));

		D3D_THROW_INFO(m_pContext->IASetInputLayout(pInputLayout.Get()));
		
		// Bind Render Target
		D3D_THROW_INFO(m_pContext->OMSetRenderTargets(1u, m_pRenderTarget.GetAddressOf(), nullptr));

		// Set Primitive Topology to Triangle List
		m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		// Config Viewport
		D3D11_VIEWPORT vp =
		{
			100,
			100,
			400,
			300,
			0,
			1
		};

		// Set Viewports
		D3D_THROW_INFO(m_pContext->RSSetViewports(1u, &vp));
		
		D3D_THROW_INFO(m_pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
	}
private:

	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget;
};