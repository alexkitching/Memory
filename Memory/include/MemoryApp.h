#pragma once

#include "WinApp.h"
#include "ScenarioManager.h"
#include "LogWindow.h"
#include "PerformanceCounterWindow.h"
#include "ScenarioWindow.h"
#include "ProfilerWindow.h"

class MemoryApp : public WinApp
{
public:
	MemoryApp();
	virtual ~MemoryApp();
	
	static void Play();
	static void Pause();
	static bool IsPaused() { return s_bPaused; }

protected:
	virtual bool Initialise() override;
	virtual void OnPreFrame() override;
	virtual void OnFrame() override;
	virtual void OnRenderFrame(IRenderer* a_pRenderer) override;
	virtual void OnGUI(IMGUIInterface& a_GUIInterface) override;
	virtual void OnPostFrame() override;
	virtual void OnExit() override;
private:
	void OnPause();
	void OnPlay();
	
	static bool s_bPaused;
	static bool s_bShouldPause;
	
	ScenarioManager m_ScenarioManager;
	LogWindow m_LogWindow;
	PerformanceCounterWindow m_PerformanceCounterWindow;
	ProfilerWindow m_ProfilerWindow;
	ScenarioWindow m_ScenarioWindow;
};
