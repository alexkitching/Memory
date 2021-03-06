#include "ResourceLoadingScenario.h"
#include "Debug.h"
#include "Profiler.h"
#include "MemoryManager.h"
#include <RandomUtility.h>

ResourceLoadingScenario::Config ResourceLoadingScenario::Configuration = {};

#if USE_MOVEABLE_HEAP
MoveableHeap* ResourceLoadingScenario::DummyResource::s_pHeap = nullptr;
#else
Heap* ResourceLoadingScenario::DummyResource::s_pHeap = nullptr;
#endif

void ResourceLoadingScenario::Run()
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::Run);
	// Tick Timers
	m_IntervalTimer.Tick();

	// Run Current Type
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

void ResourceLoadingScenario::Initialise()
{
	m_IntervalTimer.Start();
#if USE_MEM_SYS
#if USE_MOVEABLE_HEAP
	if(DummyResource::s_pHeap == nullptr)
	{
		// Create Moveable Heap
		MoveableHeap::Config config;
		config.Name = "Moveable Resource Heap";
		size_t capacity;

		if (m_CurrentType == Type::Bootup)
		{
			capacity = Configuration.Bootup.TotalSizeToLoad;
		}
		else
		{
			capacity = Configuration.Gameplay.AllocatedResourceCap;
		}

		config.Capacity = capacity + 6 * MB;
		DummyResource::s_pHeap = MemoryManager::CreateMoveableHeap(config, MemoryManager::GetDefaultHeap());
	}
#else
	if(DummyResource::s_pHeap == nullptr)
	{
		// Create Heap
		Heap::Config config{};
		config.Name = "Resource Heap";
		size_t capacity;

		if (m_CurrentType == Type::Bootup)
		{
			capacity = Configuration.Bootup.TotalSizeToLoad;
		}
		else
		{
			capacity = Configuration.Gameplay.AllocatedResourceCap;
		}

		config.Capacity = capacity * 1.25;
		DummyResource::s_pHeap = MemoryManager::CreateHeap(config, MemoryManager::GetDefaultHeap());
	}
#endif
#endif
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
	m_CurrentTotalLoadedSize = 0u;
#if USE_MEM_SYS
	MemoryManager::ReleaseHeap(DummyResource::s_pHeap);
	DummyResource::s_pHeap = nullptr;
#endif
	PROFILER_END_SAMPLE();
}

ResourceLoadingScenario::DummyResource::DummyResource(size_t a_size) :
	m_Size(a_size),
	m_pData(nullptr)
{
	char* pData = nullptr;
#if USE_MEM_SYS
#if USE_MOVEABLE_HEAP
	// Allocate from  Moveable Heap
	m_pData = s_pHeap->allocate<char>(a_size);
	pData = m_pData.Get();
#else
	m_pData = static_cast<char*>(s_pHeap->allocate(a_size));
	pData = m_pData;
#endif
#else
	m_pData = new char[a_size];
	pData = m_pData;
#endif

	if(pData == nullptr)
	{
		LOG("Failed to Allocate Data \n");
	}
	else
	{
		// Copy Data
		char str[] = "DummyResourceData";
		const size_t size = sizeof(str);

		std::memcpy(pData, str, size);
	}
}

ResourceLoadingScenario::DummyResource::~DummyResource()
{
#if USE_MOVEABLE_HEAP
	m_pData.Release();
#else
	delete[] m_pData;
#endif
}

void ResourceLoadingScenario::RunBootType()
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::RunBootType);
	if (m_IntervalTimer.IsStarted() &&
		m_IntervalTimer.GetTime() < Configuration.Bootup.LoadInterval) // Wait Interval When Set
	{
		PROFILER_END_SAMPLE();
		return;
	}

	// Load Resource
	size_t ResourceSize = Random::IntRangeWithSeed(Configuration.Bootup.MinResourceSize, Configuration.Bootup.MaxResourceSize, m_NextSeed++);
	if (m_CurrentTotalLoadedSize + ResourceSize > Configuration.Bootup.TotalSizeToLoad)
	{
		ResourceSize = Configuration.Bootup.TotalSizeToLoad - m_CurrentTotalLoadedSize; // Ensure Capacity not exceeded
	}
	
	LoadResource(ResourceSize);

	
	if (m_CurrentTotalLoadedSize >= Configuration.Bootup.TotalSizeToLoad) // Loaded All our Resources
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
		m_IntervalTimer.GetTime() < Configuration.Gameplay.LoadInterval) // Wait Interval When Set
	{
		PROFILER_END_SAMPLE();
		return;
	}

	const bool bCapacityReached = m_CurrentTotalLoadedSize >= Configuration.Gameplay.AllocatedResourceCap;
	bool bAllocateDeallocate = bCapacityReached ? false : true; // Default Allocate

	if (m_LoadedResources.empty() == false && // Potential to Deallocate
		bCapacityReached == false) // Capacity Reached - Leave to Deallocate
	{
		bAllocateDeallocate = Random::BoolWithSeed(m_NextSeed++);
	}

	if (bAllocateDeallocate)
	{
		// Load Resource
		size_t ResourceSize = Random::IntRangeWithSeed(Configuration.Bootup.MinResourceSize, Configuration.Bootup.MaxResourceSize, m_NextSeed++);
		if (m_CurrentTotalLoadedSize + ResourceSize > Configuration.Gameplay.AllocatedResourceCap) // Ensure Capacity not exceeded
		{
			ResourceSize = Configuration.Gameplay.AllocatedResourceCap - m_CurrentTotalLoadedSize;
		}
		LoadResource(ResourceSize);
	}
	else
	{
		// Free Random Resource
		int idx = 0;
		if (m_LoadedResources.size() > 1)
		{
			idx = Random::IntRangeWithSeed(0, ((int)m_LoadedResources.size()) - 1, m_NextSeed++);
		}
		FreeResource(idx);
	}

	if (m_IntervalTimer.GetTime() >= Configuration.Gameplay.LoadInterval)
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
	m_CurrentTotalLoadedSize += a_size;
	PROFILER_END_SAMPLE();
}

void ResourceLoadingScenario::FreeResource(int a_idx)
{
	PROFILER_BEGIN_SAMPLE(ResourceLoadingScenario::FreeResource);
	IDummyResource* pRes = m_LoadedResources[a_idx];
	m_CurrentTotalLoadedSize -= pRes->GetSize();
	delete pRes;
	m_LoadedResources.erase(m_LoadedResources.begin() + a_idx);
	PROFILER_END_SAMPLE();
}
