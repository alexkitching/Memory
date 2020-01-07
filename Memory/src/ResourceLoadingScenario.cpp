#include "ResourceLoadingScenario.h"

void ResourceLoadingScenario::Run()
{
	if (m_RunTimeTimer.IsStarted() == false)
	{
		m_RunTimeTimer.Start();
	}

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

}

void ResourceLoadingScenario::Reset()
{
	m_bComplete = false;
	m_CurrentType = Type::Invalid;
	m_NextSeed = 0;

	m_IntervalTimer.Stop();
	m_RunTimeTimer.Stop();

	for (auto& res : m_LoadedResources)
	{
		delete res;
	}

	m_LoadedResources.clear();
	GameplayData.m_TotalLoadedResourceSize = 0u;
}

void ResourceLoadingScenario::RunBootType()
{
	if (m_IntervalTimer.IsStarted() &&
		m_IntervalTimer.GetTime() < m_Config.Bootup.LoadInterval) // Wait Interval When Set
	{
		return;
	}

	// Load Resource
	const size_t ResourceSize = Random::IntRangeWithSeed(m_Config.Bootup.MinResourceSize, m_Config.Bootup.MaxResourceSize, m_NextSeed++);
	IDummyResource* pResource = new DummyResource(ResourceSize);
	m_LoadedResources.push_back(pResource);


	if (m_LoadedResources.size() == m_Config.Bootup.m_NumResourcesToLoad) // Loaded All our Resources
	{
		m_bComplete = true;
	}
	else // Resource Loaded - Reset Timer
	{
		m_IntervalTimer.Stop();
		m_IntervalTimer.Start();
	}
}

void ResourceLoadingScenario::RunGameplayType()
{
	if (m_IntervalTimer.IsStarted() &&
		m_IntervalTimer.GetTime() < m_Config.Gameplay.LoadInterval) // Wait Interval When Set
	{
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
		IDummyResource* pResource = new DummyResource(ResourceSize);
		m_LoadedResources.push_back(pResource);
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
		delete m_LoadedResources[idx];
		m_LoadedResources.erase(m_LoadedResources.begin() + idx);
	}

	if (m_IntervalTimer.GetTime() >= m_Config.Gameplay.LoadInterval)
	{
		m_IntervalTimer.Stop();
		m_IntervalTimer.Start();
	}

	if (m_RunTimeTimer.GetTime() >= m_Config.Gameplay.RunLength)
	{
		m_bComplete = true;
	}
}
