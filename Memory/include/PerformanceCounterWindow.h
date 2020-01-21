#pragma once

#include "IMGUIWindow.h"

//------------
// Description
//--------------
// Performance Counter Window used to Print out performance stats including heap information when using the custom memory system.
//------------

class ProfilerWindow;
class HeapBase;
class PerformanceCounterWindow : public IMGUIWindow
{
public:
	PerformanceCounterWindow(ProfilerWindow* a_pProfilerWindow);
	virtual ~PerformanceCounterWindow();

protected:
	void OnGUIWindow(const IMGUIInterface& a_interface) override;

	static void RecursivePrintHeapStats(const HeapBase* a_Heap);
private:
	ProfilerWindow* m_pProfiler;
};