#include "WinApp.h"
#include <sstream>
#include <iomanip>
#include "Debug.h"

#if DEBUG
#include "DXGI_Info_Man.h"
#endif

WinApp::WinApp()
	:
m_pWindow(nullptr),
m_Timer(false)
{
}

Window& WinApp::GetWindow() const
{
	return *m_pWindow;
}

int WinApp::Run()
{
	try
	{
		ASSERT(Initialise() && "Initialisation Failed");

		while (m_bShouldClose == false)
		{
			CycleFrame();
		}

		OnExit();

		return m_ExitCode;
	}
	catch (const Exception& e)
	{
		MessageBox(m_pWindow->GetHandle(), e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(m_pWindow->GetHandle(), e.what(), "std::Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(m_pWindow->GetHandle(), "No Details Available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return 0;
}

bool WinApp::Initialise()
{
	m_pWindow = new Window(800, 600, "Memory");

#if DEBUG
	DXGIInfoManager::Initialise();
#endif
	
	m_pWindow->Initialise();

	m_Timer.Start();
	return m_pWindow != nullptr;
}

void WinApp::OnPreFrame()
{
	// Process Windows Messages
	if(const std::optional<int> errorCode = Window::ProcessMessages())
	{
		// Optional Has Value
		SetShouldClose(*errorCode);
	}
}

void WinApp::OnFrame()
{
	// <---- Clear Render ---->
	m_pWindow->GetRenderer().Clear(0.7f, 0.7f, 0.7f);
	
	// Draw Stuff
	const float X = 1.f - (float)m_pWindow->GetMouse.GetXPos() / ((float)m_pWindow->GetWidth() * 0.5f);
	const float Y = 1.f - (float)m_pWindow->GetMouse.GetYPos() / ((float)m_pWindow->GetHeight() * 0.5f);
	
	m_pWindow->GetRenderer().DrawTestTriangle(m_Timer.GetTime(), -X, Y);
	m_pWindow->GetRenderer().DrawTestTriangle(-m_Timer.GetTime(), 0, 0);

	// <---- IMGUI ---->
	m_pWindow->GetIMGUI().BeginGUIFrame();
	OnGUI(m_pWindow->GetIMGUI());
	//m_pWindow->GetIMGUI().Test();
	m_pWindow->GetIMGUI().RenderGUIFrame();
	
	// <---- Present ---->
	m_pWindow->GetRenderer().EndFrame();
}

void WinApp::OnPostFrame()
{
	
}

void WinApp::OnExit()
{
#if DEBUG
	DXGIInfoManager::Shutdown();
#endif
}

void WinApp::CycleFrame()
{
	switch (m_FrameStage)
	{
	case FrameStage::PreFrame:
		OnPreFrame();
		m_FrameStage = FrameStage::OnFrame;
		break;
	case FrameStage::OnFrame:
		OnFrame();
		m_FrameStage = FrameStage::PostFrame;
		break;
	case FrameStage::PostFrame:
		OnPostFrame();
		m_FrameStage = FrameStage::PreFrame;
		break;
	default:
		break;
	}
}
