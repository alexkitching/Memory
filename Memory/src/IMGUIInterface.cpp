#include "IMGUIInterface.h"
#include "Debug.h"
#include "D3D.h"
#include "Window.h"
#include "../imgui-master/imgui.h"
#include "../imgui-master/examples/imgui_impl_win32.h"
#include "../imgui-master/examples/imgui_impl_dx11.h"

#include <d3d11.h>
#include "imgui_internal.h"

IMGUIInterface::IMGUIInterface()
	:
m_bInitialised(false),
m_pWindow(nullptr),
m_pDXRenderTarget(nullptr)
{
}

IMGUIInterface::~IMGUIInterface()
{
	if(m_bInitialised)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}
}

void IMGUIInterface::Initialise(Window* a_pWindow)
{
	if (m_bInitialised)
		return;

	m_pWindow = a_pWindow;

	// Setup Context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Style
	ImGui::StyleColorsDark();

	// Setup Renderer Binding
	ASSERT(ImGui_ImplWin32_Init(m_pWindow->GetHandle()));
	ASSERT(ImGui_ImplDX11_Init(m_pWindow->GetRenderer().GetDevice(), m_pWindow->GetRenderer().GetContext()));

	// Create IMGUI Render Target
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	m_pWindow->GetRenderer().GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	m_pWindow->GetRenderer().GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_pDXRenderTarget.GetAddressOf());
	
	m_bInitialised = true;
}

void IMGUIInterface::BeginGUIFrame() const
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Hook Window Input
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[0] = m_pWindow->GetMouse.LeftButtonPressed();
	io.MouseDown[1] = m_pWindow->GetMouse.RightButtonPressed();
	io.MouseDown[2] = m_pWindow->GetMouse.MiddleButtonPressed();

	for(unsigned int i = 0; i < Keyboard::s_kNumKeys; ++i)
	{
		const bool bPressed = m_pWindow->GetKeyboard.KeyPressed((WinKeyCode)i);
		
		io.KeysDown[i] = bPressed;
	}

	io.AddInputCharacter(m_pWindow->GetKeyboard.ReadChar());
}

void IMGUIInterface::RenderGUIFrame()
{
	ImGui::Render();

	// Swap Render Targets
	ID3D11RenderTargetView* pOldRT;
	ID3D11DepthStencilView* pOldDS;
	m_pWindow->GetRenderer().GetContext()->OMGetRenderTargets(1, &pOldRT, &pOldDS);
	m_pWindow->GetRenderer().GetContext()->OMSetRenderTargets(1, m_pDXRenderTarget.GetAddressOf(), NULL);

	// Render to Target
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Swap Render Target Back
	m_pWindow->GetRenderer().GetContext()->OMSetRenderTargets(1, &pOldRT, pOldDS);
}

bool IMGUIInterface::Button(const char* a_pName, bool a_bEnabled) const
{
	if(a_bEnabled == false) // Disabled
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	const bool bPressed = ImGui::Button(a_pName);

	if(a_bEnabled == false)
	{
		ImGui::PopStyleVar();
		ImGui::PopItemFlag();
	}

	return bPressed;
}

bool IMGUIInterface::IntSlider(const char* a_pName, bool a_bEnabled,
	int* a_pData, float a_Speed, int a_Min,
	int a_Max, const char* a_Format) const
{
	if (a_bEnabled == false)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	
	const bool bPressed = ImGui::DragInt(a_pName, a_pData, a_Speed, a_Min, a_Max, a_Format);

	if (a_bEnabled == false)
	{
		ImGui::PopStyleVar();
		ImGui::PopItemFlag();
	}

	return bPressed;
}

bool IMGUIInterface::FloatSlider(const char* a_pName, bool a_bEnabled, float* a_pData, float a_Speed, float a_Min,
	float a_Max, const char* a_Format, float a_Power) const
{
	if (a_bEnabled == false)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	const bool bPressed = ImGui::DragFloat(a_pName, a_pData, a_Speed, a_Min, a_Max, a_Format, a_Power);

	if (a_bEnabled == false)
	{
		ImGui::PopStyleVar();
		ImGui::PopItemFlag();
	}

	return bPressed;
}
