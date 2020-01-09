#include "PerformanceCounterWindow.h"
#include "Win.h"
#include "psapi.h"

#include <imgui.h>
#include "GlobalTime.h"
#include "MemoryManager.h"
#include "Heap.h"
#include "MemSys.h"

PerformanceCounterWindow::PerformanceCounterWindow()
	:
IMGUIWindow("Performance Counter"),
m_fDeltaTime(0.f),
m_fAccumFPS(0.f),
m_FrameCount(0),
m_fFPS(0.f)
{
}

PerformanceCounterWindow::~PerformanceCounterWindow()
{
}

void PerformanceCounterWindow::OnGUIWindow(const IMGUIInterface& a_interface)
{
	m_fDeltaTime = Time::DeltaTime();
	m_fAccumFPS += m_fDeltaTime;
	m_FrameCount++;
	
	// Update FPS
	if (m_fAccumFPS > 1.0 / m_fFPSUpdateRate)
	{
		m_fFPS = m_FrameCount / m_fAccumFPS;
		m_FrameCount = 0;
		m_fAccumFPS -= 1.0f / m_fFPSUpdateRate;
	}
	
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	const DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
	const DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
	const SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	const SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	

	ImGui::Text("FPS: %.1f", m_fFPS);
	ImGui::Text("CPU Time: %.3f ms", 1000.f / m_fFPS);

	ImGui::Separator();
	ImGui::Text("Physical Memory Usage: %.1f Mbs", (float)((float)physMemUsedByMe / MB));
	ImGui::Text("Physical Memory Capacity: %.1f Mbs", (float)((float)totalPhysMem / MB));
	ImGui::Separator();
	ImGui::Text("Virtual Memory Usage: %.1f Mbs", (float)((float)virtualMemUsedByMe / MB));
	ImGui::Text("Virtual Memory Capacity: %.1f Mbs", (float)((float)totalVirtualMem / MB));
	ImGui::Separator();
#if USE_MEM_SYS
	ImGui::Text("Default Heap Usage: %.1f Mbs", (float)((float)MemoryManager::GetDefaultHeap()->GetTotalAllocationSize()/ MB));
#endif
}
