#include "MemoryApp.h"
#include "Debug.h"
#include "Profiler.h"
#include "MemSys.h"

#if USE_MEM_SYS
#include "MemoryManager.h"
#endif

bool MemoryApp::s_bPaused = false;
bool MemoryApp::s_bShouldPause = false;

MemoryApp::MemoryApp()
	:
m_PerformanceCounterWindow(&m_ProfilerWindow),
m_ScenarioWindow(&m_ScenarioManager)
{
}

MemoryApp::~MemoryApp()
{
}

bool MemoryApp::Initialise()
{
	// Init Window
	ASSERT(WinApp::Initialise() && "WinApp Init Failed!");

	m_ProfilerWindow.Initialise();
	
	return true;
}

void MemoryApp::OnPreFrame()
{	
	PROFILER_BEGIN_SAMPLE(MemoryApp::OnPreFrame);

	// Call Base
	WinApp::OnPreFrame();

	if (s_bPaused)
	{
		PROFILER_END_SAMPLE();
		return;
	}

	// Preframe Stuff Here

	PROFILER_END_SAMPLE();
}

void MemoryApp::OnFrame()
{
	PROFILER_BEGIN_SAMPLE(MemoryApp::OnFrame);
	
	if (s_bPaused)
	{
		PROFILER_END_SAMPLE();
		return;
	}
	
	WinApp::OnFrame();
	
	m_ScenarioManager.Update(); // Tick Scenarios
	PROFILER_END_SAMPLE();
}

void MemoryApp::OnRenderFrame(IRenderer* a_pRenderer)
{
	PROFILER_BEGIN_SAMPLE(MemoryApp::OnRenderFrame);
	
	m_ScenarioManager.OnRender(a_pRenderer);

	WinApp::OnRenderFrame(a_pRenderer);
	PROFILER_END_SAMPLE();
}

void MemoryApp::OnGUI(IMGUIInterface& a_GUIInterface)
{
	PROFILER_BEGIN_SAMPLE(MemoryApp::OnGUI);
	// Draw IMGUI Stuff Here
	m_LogWindow.OnGUI(a_GUIInterface);
	m_PerformanceCounterWindow.OnGUI(a_GUIInterface);
	m_ProfilerWindow.OnGUI(a_GUIInterface);
	m_ScenarioWindow.OnGUI(a_GUIInterface);
	PROFILER_END_SAMPLE();
}

void MemoryApp::OnPostFrame()
{
	PROFILER_BEGIN_SAMPLE(MemoryApp::OnPostFrame);

	WinApp::OnPostFrame();

#if USE_MEM_SYS // Defrag Moveable Heaps
	MemoryManager::DefragmentHeaps();
#endif

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
	PROFILER_END_SAMPLE();
}

void MemoryApp::OnExit()
{
	WinApp::OnExit();
}

void MemoryApp::OnPause()
{
	s_bPaused = true;
}

void MemoryApp::OnPlay()
{
	s_bPaused = false;
}
