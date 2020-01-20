#pragma once

#include "WinApp.h"
#include "ScenarioManager.h"
#include "LogWindow.h"
#include "PerformanceCounterWindow.h"
#include "ScenarioWindow.h"
#include "ProfilerWindow.h"

//------------
// Description
//--------------
// Memory Application Class, contains the main functionality required for the IRP project.
//------------

class MemoryApp : public WinApp
{
public:
	MemoryApp();
	virtual ~MemoryApp();

	// Static Functions for Playing/Pausing, actual Pause/Play Occurs just before next frame
	static void Play() { s_bShouldPause = false; }
	static void Pause() { s_bShouldPause = true; }
	static bool IsPaused() { return s_bPaused; }

protected:
	virtual bool Initialise() override;

	// Frame Stages
	virtual void OnPreFrame() override;
	virtual void OnFrame() override;
	virtual void OnRenderFrame(IRenderer* a_pRenderer) override;
	virtual void OnPostFrame() override;

	virtual void OnExit() override;

	virtual void OnGUI(IMGUIInterface& a_GUIInterface) override;
private:

	// Pausing
	void OnPause();
	void OnPlay();
	
	static bool s_bPaused;
	static bool s_bShouldPause;

	// Windows
	ScenarioManager m_ScenarioManager;
	LogWindow m_LogWindow;
	PerformanceCounterWindow m_PerformanceCounterWindow;
	ProfilerWindow m_ProfilerWindow;
	ScenarioWindow m_ScenarioWindow;
};
