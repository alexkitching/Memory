#pragma once
#include "Win.h"
#include <wrl.h>

//------------
// Description
//--------------
// IMGUI Interface Instance, should have probably refactored this into a D3D IMGUI interface from a base
// Used as an override for existing imgui functionality with enhancements.
//------------

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

	void BeginGUIFrame() const;
	void RenderGUIFrame();

	bool Button(const char* a_pName, bool a_bEnabled = true) const;
	bool IntSlider(const char* a_pName, bool a_bEnabled,
					int* a_pData, float a_Speed, int a_Min,
				int a_Max, const char* a_Format = "%d") const;
	bool FloatSlider(const char* a_pName, bool a_bEnabled,
		float* a_pData, float a_Speed, float a_Min,
		float a_Max, const char* a_Format = "%.3f", float a_Power = 1) const;
	
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