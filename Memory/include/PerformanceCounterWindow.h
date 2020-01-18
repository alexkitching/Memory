#pragma once

#include "IMGUIWindow.h"

class ProfilerWindow;
class HeapBase;
class PerformanceCounterWindow : public IMGUIWindow
{
public:
	PerformanceCounterWindow(ProfilerWindow* a_pProfilerWindow);
	virtual ~PerformanceCounterWindow();

protected:
	void OnGUIWindow(const IMGUIInterface& a_interface) override;

	void RecursivePrintHeapStats(const HeapBase* a_Heap) const;
private:
	ProfilerWindow* m_pProfiler;
};