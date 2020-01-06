#pragma once

#include "WinApp.h"
#include "ScenarioManager.h"
#include "LogWindow.h"
#include "PerformanceCounterWindow.h"

class MemoryApp : public WinApp
{
public:
	MemoryApp();
	virtual ~MemoryApp();

protected:
	virtual bool Initialise() override;
	virtual void OnPreFrame() override;
	virtual void OnFrame() override;
	virtual void OnGUI(IMGUIInterface& a_GUIInterface) override;
	virtual void OnPostFrame() override;
	virtual void OnExit() override;
private:
	ScenarioManager m_ScenarioManager;
	LogWindow m_LogWindow;
	PerformanceCounterWindow m_PerformanceCounterWindow;
};