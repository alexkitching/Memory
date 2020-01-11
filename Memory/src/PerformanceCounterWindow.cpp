#include "PerformanceCounterWindow.h"
#include "PerformanceCounter.h"
#include "Win.h"
#include "psapi.h"

#include <imgui.h>
#include "GlobalTime.h"
#include "MemoryManager.h"
#include "Heap.h"
#include "MemSys.h"

PerformanceCounterWindow::PerformanceCounterWindow()
	:
IMGUIWindow("Performance Counter")
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
#endif
}
