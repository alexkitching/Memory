#include "ResourceLoadingScenario.h"
#include "Debug.h"
#include "Profiler.h"

void ResourceLoadingScenario::Run()
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::Run);
	// Tick Timers
	m_IntervalTimer.Tick();

	switch (m_CurrentType)
	{
	case Type::Bootup:
		RunBootType();
		break;
	case Type::Gameplay:
		RunGameplayType();
		break;
	default:
		m_bComplete = true;
		break;
	}
	PROFILER_END_SAMPLE();
}

void ResourceLoadingScenario::Reset()
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::Reset);
	m_bComplete = false;
	m_CurrentType = Type::Invalid;
	m_NextSeed = 0;

	m_IntervalTimer.Stop();

	for (auto& res : m_LoadedResources)
	{
		delete res;
	}

	m_LoadedResources.clear();
	m_TotalLoadedSize = 0;
	GameplayData.m_TotalLoadedResourceSize = 0u;
	PROFILER_END_SAMPLE();
}

void ResourceLoadingScenario::RunBootType()
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::RunBootType);
	if (m_IntervalTimer.IsStarted() &&
		m_IntervalTimer.GetTime() < m_Config.Bootup.LoadInterval) // Wait Interval When Set
	{
		PROFILER_END_SAMPLE();
		return;
	}

	// Load Resource
	size_t ResourceSize = Random::IntRangeWithSeed(m_Config.Bootup.MinResourceSize, m_Config.Bootup.MaxResourceSize, m_NextSeed++);
	if (m_TotalLoadedSize + ResourceSize > m_Config.Bootup.TotalSizeToLoad)
	{
		ResourceSize = m_Config.Bootup.TotalSizeToLoad - m_TotalLoadedSize;
	}
	LoadResource(ResourceSize);

	
	if (m_TotalLoadedSize >= m_Config.Bootup.TotalSizeToLoad) // Loaded All our Resources
	{
		m_bComplete = true;
	}
	else // Resource Loaded - Reset Timer
	{
		m_IntervalTimer.Stop();
		m_IntervalTimer.Start();
	}
	PROFILER_END_SAMPLE();
}

void ResourceLoadingScenario::RunGameplayType()
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::RunGameplayType);
	if (m_IntervalTimer.IsStarted() &&
		m_IntervalTimer.GetTime() < m_Config.Gameplay.LoadInterval) // Wait Interval When Set
	{
		PROFILER_END_SAMPLE();
		return;
	}

	const bool bCapacityReached = GameplayData.m_TotalLoadedResourceSize >= m_Config.Gameplay.MaxAllocatedResourceSize;
	bool bAllocateDeallocate = bCapacityReached ? false : true; // Default Allocate

	if (m_LoadedResources.empty() == false && // Potential to Deallocate
		bCapacityReached == false) // Capacity Reached - Leave to Deallocate
	{
		bAllocateDeallocate = Random::BoolWithSeed(m_NextSeed++);
	}

	if (bAllocateDeallocate)
	{
		// Load Resource
		const size_t ResourceSize = Random::IntRangeWithSeed(m_Config.Bootup.MinResourceSize, m_Config.Bootup.MaxResourceSize, m_NextSeed++);
		LoadResource(ResourceSize);
		GameplayData.m_TotalLoadedResourceSize += ResourceSize;
	}
	else
	{
		// Free Random Resource
		int idx = 0;
		if (m_LoadedResources.size() > 1)
		{
			idx = Random::IntRangeWithSeed(0, ((int)m_LoadedResources.size()) - 1, m_NextSeed++);
		}

		GameplayData.m_TotalLoadedResourceSize -= m_LoadedResources[idx]->GetSize();
		FreeResource(idx);
	}

	if (m_IntervalTimer.GetTime() >= m_Config.Gameplay.LoadInterval)
	{
		m_IntervalTimer.Stop();
		m_IntervalTimer.Start();
	}

	PROFILER_END_SAMPLE();
}

void ResourceLoadingScenario::LoadResource(size_t a_size)
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::LoadResource);
	IDummyResource* pResource = new DummyResource(a_size);
	m_LoadedResources.push_back(pResource);
	m_TotalLoadedSize += a_size;
	PROFILER_END_SAMPLE();
}

void ResourceLoadingScenario::FreeResource(int a_idx)
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::FreeResource);
	delete m_LoadedResources[a_idx];
	m_LoadedResources.erase(m_LoadedResources.begin() + a_idx);
	PROFILER_END_SAMPLE();
}
