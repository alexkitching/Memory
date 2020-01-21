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
	// Draw Stats
	ImGui::Text("FPS: %.1f", PerformanceCounter::FPS());
	ImGui::Text("CPU Time: %.3f ms", 1000.f / PerformanceCounter::FPS());

	ImGui::Separator();
	ImGui::Text("Physical Memory Usage: %.1f Mbs", PerformanceCounter::PhysicalMemoryUsed());
	ImGui::Text("Physical Memory Capacity: %.1f Mbs", PerformanceCounter::PhysicalMemoryTotal());
	ImGui::Separator();
	ImGui::Text("Virtual Memory Usage: %.1f Mbs", PerformanceCounter::VirtualMemoryUsed());
	ImGui::Text("Virtual Memory Capacity: %.1f Mbs", PerformanceCounter::VirtualMemoryTotal());
	ImGui::Separator();

	// Print Heap Stats
	
#if USE_MEM_SYS
	RecursivePrintHeapStats(MemoryManager::GetDefaultHeap());
	
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

void PerformanceCounterWindow::RecursivePrintHeapStats(const HeapBase* a_Heap)
{
	ImGui::Text("Heap:%s Usage: %.1f Mbs", a_Heap->GetName(), (float)((float)a_Heap->GetUsedMemory() / MB));
	ImGui::Text("Heap:%s Fragmentation: %.1f %s", a_Heap->GetName(), a_Heap->CalculateFragmentation(), "%");

	if(a_Heap->GetNextSibling() != nullptr)
	{
		RecursivePrintHeapStats(a_Heap->GetNextSibling());
	}

	if(a_Heap->GetChild() != nullptr)
	{
		RecursivePrintHeapStats(a_Heap->GetChild());
	}
}
