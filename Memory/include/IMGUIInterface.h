#pragma once
#include "Win.h"
#include <wrl.h>

class Window;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
class IMGUIInterface
{
public:
	IMGUIInterface();
	~IMGUIInterface();
	void Initialise(Window* a_pWindow);

	void BeginGUIFrame();
	void Test();
	void RenderGUIFrame();

	bool Button(const char* a_pName, bool a_bEnabled = true) const;

private:
	bool m_bInitialised;
	
	Window* m_pWindow;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pDXRenderTarget;
};

struct IMGUIQuad
{
	float x;
	float y;

	float w;
	float h;

	float r;
	float g;
	float b;
	float a;
};