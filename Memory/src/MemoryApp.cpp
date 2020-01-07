#include "MemoryApp.h"
#include "Debug.h"

MemoryApp::MemoryApp()
	:
m_ScenarioManager(ScenarioManager()),
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
	return true;
}

void MemoryApp::OnPreFrame()
{
	WinApp::OnPreFrame();
}

void MemoryApp::OnFrame()
{
	WinApp::OnFrame();
	m_ScenarioManager.Update();
	
}

void MemoryApp::OnGUI(IMGUIInterface& a_GUIInterface)
{
	// Draw IMGUI Stuff Here
	m_LogWindow.OnGUI(a_GUIInterface);
	m_PerformanceCounterWindow.OnGUI(a_GUIInterface);
	m_ScenarioWindow.OnGUI(a_GUIInterface);
}

void MemoryApp::OnPostFrame()
{
	WinApp::OnPostFrame();
}

void MemoryApp::OnExit()
{
	WinApp::OnExit();
}
