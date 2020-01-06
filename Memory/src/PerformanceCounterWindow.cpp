#include "PerformanceCounterWindow.h"
#include "Win.h"
#include "psapi.h"

#include <imgui.h>
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

	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("CPU Time: %.3f ms", 1000.f / ImGui::GetIO().Framerate);

	ImGui::Separator();
	ImGui::Text("Physical Memory Usage: %.1f Mbs", (float)((float)physMemUsedByMe / MB));
	ImGui::Text("Physical Memory Capacity: %.1f Mbs", (float)((float)totalPhysMem / MB));
	ImGui::Separator();
	ImGui::Text("Virtual Memory Usage: %.1f Mbs", (float)((float)virtualMemUsedByMe / MB));
	ImGui::Text("Virtual Memory Capacity: %.1f Mbs", (float)((float)totalVirtualMem / MB));
	
}
