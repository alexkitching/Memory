#include "MemoryApp.h"
#include "Debug.h"
#include "Profiler.h"

bool MemoryApp::s_bPaused = false;
bool MemoryApp::s_bShouldPause = false;

MemoryApp::MemoryApp()
	:
m_ScenarioManager(ScenarioManager()),
m_ScenarioWindow(&m_ScenarioManager)
{
}

MemoryApp::~MemoryApp()
{
}

void MemoryApp::Play()
{
	s_bShouldPause = false;
}

void MemoryApp::Pause()
{
	s_bShouldPause = true;
}

bool MemoryApp::Initialise()
{
	// Init Window
	ASSERT(WinApp::Initialise() && "WinApp Init Failed!");
	
	Profiler::Initialise();
	m_ProfilerWindow.Initialise();
	
	return true;
}

void MemoryApp::OnPreFrame()
{
	Profiler::OnFrameStart();
	PROFILER_BEGIN_SAMPLE(WinApp::OnPreFrame);
	WinApp::OnPreFrame();
	PROFILER_END_SAMPLE();
	if (s_bPaused)
		return;
}

void MemoryApp::OnFrame()
{
	if (s_bPaused)
		return;
	
	WinApp::OnFrame();

	PROFILER_BEGIN_SAMPLE(ScenarioManager::Update);
	m_ScenarioManager.Update();
	PROFILER_END_SAMPLE();
}

void MemoryApp::OnRenderFrame(IRenderer* a_pRenderer)
{
	PROFILER_BEGIN_SAMPLE(WinApp::OnRenderFrame);
	WinApp::OnRenderFrame(a_pRenderer);
	PROFILER_END_SAMPLE();
	
	PROFILER_BEGIN_SAMPLE(ScenarioManager::OnRender);
	m_ScenarioManager.OnRender(a_pRenderer);
	PROFILER_END_SAMPLE();
}

void MemoryApp::OnGUI(IMGUIInterface& a_GUIInterface)
{
	// Draw IMGUI Stuff Here
	m_LogWindow.OnGUI(a_GUIInterface);
	m_PerformanceCounterWindow.OnGUI(a_GUIInterface);
	m_ProfilerWindow.OnGUI(a_GUIInterface);
	m_ScenarioWindow.OnGUI(a_GUIInterface);
}

void MemoryApp::OnPostFrame()
{
	if (s_bPaused == false)
	{
		WinApp::OnPostFrame();

	}
	Profiler::OnFrameEnd();

	if(s_bPaused != s_bShouldPause)
	{
		if(s_bShouldPause)
		{
			OnPause();
		}
		else
		{
			OnPlay();
		}
	}
}

void MemoryApp::OnExit()
{
	WinApp::OnExit();
}

void MemoryApp::OnPause()
{
	s_bPaused = true;
	Profiler::Pause();
}

void MemoryApp::OnPlay()
{
	s_bPaused = false;
	Profiler::Play();
}
