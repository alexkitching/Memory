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
m_Result(0),
m_FrameStage(FrameStage::PreFrame),
m_bShouldClose(false),
m_iExitCode(0)
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

		while (m_bShouldClose == false) // Run Program
		{
			CycleFrame();
		}

		OnExit();

		return m_iExitCode;
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
	Profiler::Initialise();
	
	m_pWindow = new Window(1280, 768, "Memory");

#if DEBUG
	DXGIInfoManager::Initialise();
#endif
	
	m_pWindow->Initialise();

	return m_pWindow != nullptr;
}

void WinApp::OnPreFrame()
{
	PROFILER_BEGIN_SAMPLE(WinApp::OnPreFrame);
	// Process Windows Messages
	if(const std::optional<int> errorCode = Window::ProcessMessages()) // Recieved Error Code
	{
		// Optional Has Value
		SetShouldClose(*errorCode);
	}
	
	PerformanceCounter::Tick();
	PROFILER_END_SAMPLE();
}

float MouseX;
float MouseY;
float Angle;

void WinApp::OnFrame()
{
	PROFILER_BEGIN_SAMPLE(WinApp::OnFrame);
	MouseX = 1.f - (float)m_pWindow->GetMouse.GetXPos() / ((float)m_pWindow->GetWidth() * 0.5f);
	MouseY = 1.f - (float)m_pWindow->GetMouse.GetYPos() / ((float)m_pWindow->GetHeight() * 0.5f);

	Angle -= Time::DeltaTime();

	PROFILER_END_SAMPLE();
}



void WinApp::OnPreRenderFrame()
{
	PROFILER_BEGIN_SAMPLE(WinApp::OnPreRenderFrame);
	// <---- Clear Render ---->
	m_pWindow->GetRenderer().Clear(0.7f, 0.7f, 0.7f);
	PROFILER_END_SAMPLE();
}

void WinApp::OnRenderFrame(IRenderer* a_pRenderer)
{
	// Draw Stuff
	PROFILER_BEGIN_SAMPLE(WinApp::OnRenderFrame);

	// Draw Visual Cubes for Visible Stutters
	m_pWindow->GetRenderer().DrawCube(0.f, 0.f, 0.f, 1.f, Angle);
	m_pWindow->GetRenderer().DrawCube(3.f, 0.f, 0.f, 0.5f, Angle);
	
	// <---- IMGUI ---->
	m_pWindow->GetIMGUI().BeginGUIFrame();
	OnGUI(m_pWindow->GetIMGUI());
	m_pWindow->GetIMGUI().RenderGUIFrame();

	PROFILER_END_SAMPLE();
}

void WinApp::OnPostRenderFrame()
{
	PROFILER_BEGIN_SAMPLE(WinApp::OnPostRenderFrame);
	// <---- Present ---->
	m_pWindow->GetRenderer().EndFrame();
	PROFILER_END_SAMPLE();
}

void WinApp::OnPostFrame()
{
	
}

void WinApp::OnExit()
{
	Profiler::Shutdown();
#if DEBUG
	DXGIInfoManager::Shutdown();
#endif
	delete m_pWindow;
}

void WinApp::CycleFrame()
{
	// Runs Current Frame Stage
	switch (m_FrameStage)
	{
	case FrameStage::PreFrame:
		Profiler::OnFrameStart();
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
		Profiler::OnFrameEnd();
		break;
	default:
		break;
	}
}
