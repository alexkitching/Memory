#include "WinApp.h"
#include "Debug.h"
#include "GlobalTime.h"
#include <iomanip>
#include "PerformanceCounter.h"
#include "Profiler.h"

#if DEBUG
#include "DXGI_Info_Man.h"
#endif

WinApp::WinApp()
	:
m_pWindow(nullptr),
m_FrameStage(FrameStage::PreFrame),
m_bShouldClose(false)
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
	m_pWindow = new Window(1280, 768, "Memory");

#if DEBUG
	DXGIInfoManager::Initialise();
#endif
	
	m_pWindow->Initialise();

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
	PROFILER_BEGIN_SAMPLE(PerformanceCounter::Tick);
	PerformanceCounter::Tick();
	PROFILER_END_SAMPLE();

	PROFILER_BEGIN_SAMPLE(PerformanceCounter::Tick);
	//PerformanceCounter::Tick();
	PROFILER_END_SAMPLE();
	
}

float MouseX;
float MouseY;
float Angle;

void WinApp::OnFrame()
{
	MouseX = 1.f - (float)m_pWindow->GetMouse.GetXPos() / ((float)m_pWindow->GetWidth() * 0.5f);
	MouseY = 1.f - (float)m_pWindow->GetMouse.GetYPos() / ((float)m_pWindow->GetHeight() * 0.5f);

	Angle -= Time::DeltaTime();
}



void WinApp::OnPreRenderFrame()
{
	// <---- Clear Render ---->
	m_pWindow->GetRenderer().Clear(0.7f, 0.7f, 0.7f);
}

void WinApp::OnRenderFrame(IRenderer* a_pRenderer)
{
	// Draw Stuff
	
	const float zOffs = 0.f;
	//m_pWindow->GetRenderer().DrawCube(-MouseX, 0.f, zOffs + MouseY, Time::TimeSinceStartup());
	m_pWindow->GetRenderer().DrawCube(0.f, 0.f, zOffs, 1.f, Angle);
	m_pWindow->GetRenderer().DrawCube(3.f, 0.f, zOffs, 0.5f, Angle);
	// <---- IMGUI ---->
	m_pWindow->GetIMGUI().BeginGUIFrame();
	OnGUI(m_pWindow->GetIMGUI());
	//m_pWindow->GetIMGUI().Test();
	m_pWindow->GetIMGUI().RenderGUIFrame();

	
}

void WinApp::OnPostRenderFrame()
{
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
		m_Time.Update();
		OnPreFrame();
		m_FrameStage = FrameStage::OnFrame;
		break;
	case FrameStage::OnFrame:
		OnFrame();
		m_FrameStage = FrameStage::OnRenderFrame;
		break;
	case FrameStage::OnRenderFrame:
		OnPreRenderFrame();
		OnRenderFrame(&m_pWindow->GetRenderer());
		OnPostRenderFrame();
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
