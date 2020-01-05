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


	return true;
}

void MemoryApp::OnPreFrame()
{
	WinApp::OnPreFrame();
}

void MemoryApp::OnFrame()
{
	WinApp::OnFrame();
}

void MemoryApp::OnGUI(IMGUIInterface& a_GUIInterface)
{
	// Draw IMGUI Stuff Here
}

void MemoryApp::OnPostFrame()
{
	WinApp::OnPostFrame();
}

void MemoryApp::OnExit()
{
	WinApp::OnExit();
}
