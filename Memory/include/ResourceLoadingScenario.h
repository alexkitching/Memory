#pragma once
#include "Scenario.h"
#include "Timer.h"
#include "RandomUtility.h"
#include <vector>

class ResourceLoadingScenario : public IScenario
{
public:
	struct Config
	{
		// Bootup Config
		struct TypeConfig
		{
			float LoadInterval;
			size_t MinResourceSize;
			size_t MaxResourceSize;
			size_t m_NumResourcesToLoad;
		};
		
		TypeConfig Bootup;
		TypeConfig Gameplay;
	};

	enum class Type
	{
		Invalid = -1,
		Bootup,
		Gameplay
	};

	// Constructors
	ResourceLoadingScenario() = default;
	ResourceLoadingScenario(const Config& a_config)
		:
		m_bComplete(false),
		m_CurrentType(Type::Invalid),
		m_Config(a_config),
		m_NextSeed(0)
	{}
	~ResourceLoadingScenario() {}

	// Functions
	void Run() override
	{
		switch(m_CurrentType)
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
	void Reset() override
	{
		m_bComplete = false;
		m_CurrentType = Type::Invalid;
		m_NextSeed = 0;
		m_Timer.Stop();

		for(auto& res : m_LoadedResources)
		{
			delete res;
		}

		m_LoadedResources.clear();
	}
	bool IsComplete() override { return m_bComplete; }
	
	void SetType(Type a_type) { m_CurrentType = a_type;  }
	
private:
	class IDummyResource
	{
	public:
		virtual ~IDummyResource() {}
	};

	class DummyResource : public IDummyResource
	{
	public:
		DummyResource(size_t a_size)
			:
			m_pData(nullptr)
		{
			m_pData = new char[a_size];
		}

		virtual ~DummyResource()
		{
			delete[] m_pData;
		}

	private:
		char* m_pData;
	};
	
	void RunBootType()
	{
		if(m_Timer.IsStarted() &&
			m_Timer.GetTime() < m_Config.Bootup.LoadInterval) // Wait Interval When Set
		{
			return;
		}

		// Load Resource
		const size_t ResourceSize = Random::IntRangeWithSeed(m_Config.Bootup.MinResourceSize, m_Config.Bootup.MaxResourceSize, m_NextSeed++);

		IDummyResource* pResource = new DummyResource(ResourceSize);
		m_LoadedResources.push_back(pResource);
		

		if(m_LoadedResources.size() == m_Config.Bootup.m_NumResourcesToLoad) // Loaded All our Resources
		{
			m_bComplete = true;
		}
		else // Resource Loaded - Reset Timer
		{
			m_Timer.Stop();
			m_Timer.Start();
		}
	}

	void RunGameplayType()
	{
		if (m_LoadedResources.size() == m_Config.Gameplay.m_NumResourcesToLoad)
		{
			m_bComplete = true;
		}
	}

	bool m_bComplete;
	Type m_CurrentType;
	Config m_Config;

	Timer m_Timer;

	int m_NextSeed;

	std::vector<IDummyResource*> m_LoadedResources;
};