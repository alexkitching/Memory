#include "MemoryApp.h"
#include "Debug.h"

MemoryApp::MemoryApp()
{
}

MemoryApp::~MemoryApp()
{
}

bool MemoryApp::Initialise()
{
	// Init Window
	ASSERT(WinApp::Initialise() && "WinApp Init Failed!");

	m_ScenarioManager.StartScenario(ScenarioManager::ScenarioType::ResourceLoadingBootup);

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
}

void MemoryApp::OnPostFrame()
{
	WinApp::OnPostFrame();
}

void MemoryApp::OnExit()
{
	WinApp::OnExit();
}
