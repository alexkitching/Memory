#pragma once

#include "IMGUIWindow.h"

class ProfilerWindow;
class PerformanceCounterWindow : public IMGUIWindow
{
public:
	PerformanceCounterWindow(ProfilerWindow* a_pProfilerWindow);
	virtual ~PerformanceCounterWindow();

protected:
	void OnGUIWindow(const IMGUIInterface& a_interface) override;

private:
	ProfilerWindow* m_pProfiler;
};