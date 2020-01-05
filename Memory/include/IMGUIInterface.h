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

	void OnBeginFrame();
	void Test();
	void OnPostFrame();
private:
	bool m_bInitialised;
	
	Window* m_pWindow;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pDXRenderTarget;
};