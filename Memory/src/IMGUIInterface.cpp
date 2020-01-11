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

void IMGUIInterface::BeginGUIFrame()
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
		bool bPressed = m_pWindow->GetKeyboard.KeyPressed((WinKeyCode)i);
		
		io.KeysDown[i] = bPressed;
	}

	io.AddInputCharacter(m_pWindow->GetKeyboard.ReadChar());
}

char buf[256];
int counter;

void IMGUIInterface::Test()
{
	ImGui::Begin("Hello, world!");

	ImGui::Text("This is some useful text.");

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	
	
	ImGui::InputText("Input Text:", buf, sizeof(buf));
	
	ImGui::End();
}

void IMGUIInterface::RenderGUIFrame()
{
	ImGui::Render();
	ID3D11RenderTargetView* pOldRT;
	ID3D11DepthStencilView* pOldDS;
	m_pWindow->GetRenderer().GetContext()->OMGetRenderTargets(1, &pOldRT, &pOldDS);
	m_pWindow->GetRenderer().GetContext()->OMSetRenderTargets(1, m_pDXRenderTarget.GetAddressOf(), NULL);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_pWindow->GetRenderer().GetContext()->OMSetRenderTargets(1, &pOldRT, pOldDS);
}

bool IMGUIInterface::Button(const char* a_pName, bool a_bEnabled) const
{
	if(a_bEnabled == false)
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
