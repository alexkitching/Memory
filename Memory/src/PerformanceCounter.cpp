#include "PerformanceCounter.h"
#include "GlobalTime.h"
#include "Win.h"
#include <psapi.h>


float PerformanceCounter::s_fFPS = 0.f;
float PerformanceCounter::s_fDeltaTime = 0.f;
float PerformanceCounter::s_fAccumFPS = 0.f;
int PerformanceCounter::s_FrameCount = 0;
float PerformanceCounter::s_fPhysMemUsed = 0.f;
float PerformanceCounter::s_fPhysMemTotal = 0.f;
float PerformanceCounter::s_fVirMemUsed = 0.f;
float PerformanceCounter::s_fVirMemTotal = 0.f;

void PerformanceCounter::Tick()
{
	s_fDeltaTime = Time::DeltaTime();
	s_fAccumFPS += s_fDeltaTime;
	s_FrameCount++;

	// Update FPS
	if (s_fAccumFPS > 1.0 / s_fFPSUpdateRate)
	{
		s_fFPS = s_FrameCount / s_fAccumFPS;
		s_FrameCount = 0;
		s_fAccumFPS -= 1.0f / s_fFPSUpdateRate;
	}

	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	s_fVirMemTotal = (float)memInfo.ullTotalPageFile / MB;
	s_fPhysMemTotal = (float)memInfo.ullTotalPhys / MB;

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
	s_fVirMemUsed = (float)pmc.PrivateUsage / MB;
	s_fPhysMemUsed = (float)pmc.WorkingSetSize / MB;
}
