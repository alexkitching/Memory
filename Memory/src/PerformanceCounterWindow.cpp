#include "PerformanceCounterWindow.h"
#include "PerformanceCounter.h"
#include "MemoryManager.h"
#include "Heap.h"
#include "MemSys.h"
#include "ProfilerWindow.h"
#include <imgui.h>

PerformanceCounterWindow::PerformanceCounterWindow(ProfilerWindow* a_pProfilerWindow)
	:
IMGUIWindow("Performance Counter", false),
m_pProfiler(a_pProfilerWindow)
{
}

PerformanceCounterWindow::~PerformanceCounterWindow()
{
}

void PerformanceCounterWindow::OnGUIWindow(const IMGUIInterface& a_interface)
{
	ImGui::Text("FPS: %.1f", PerformanceCounter::FPS());
	ImGui::Text("CPU Time: %.3f ms", 1000.f / PerformanceCounter::FPS());

	ImGui::Separator();
	ImGui::Text("Physical Memory Usage: %.1f Mbs", PerformanceCounter::PhysicalMemoryUsed());
	ImGui::Text("Physical Memory Capacity: %.1f Mbs", PerformanceCounter::PhysicalMemoryTotal());
	ImGui::Separator();
	ImGui::Text("Virtual Memory Usage: %.1f Mbs", PerformanceCounter::VirtualMemoryUsed());
	ImGui::Text("Virtual Memory Capacity: %.1f Mbs", PerformanceCounter::VirtualMemoryTotal());
	ImGui::Separator();
	
#if USE_MEM_SYS 
	ImGui::Text("Default Heap Usage: %.1f Mbs", (float)((float)MemoryManager::GetDefaultHeap()->GetUsedMemory()/ MB));
	ImGui::Text("Default Heap Fragmentation: %.1f %s", MemoryManager::GetDefaultHeap()->CalculateFragmentation(), "%");
	ImGui::Separator();
#endif
	
	if(a_interface.Button("Toggle Profiler"))
	{
		if(m_pProfiler->IsOpen())
		{
			m_pProfiler->Close();
		}
		else
		{
			m_pProfiler->Open();
		}
	}
	
}
