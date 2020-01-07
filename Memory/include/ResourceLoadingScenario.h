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
		struct
		{
			float LoadInterval;
			size_t MinResourceSize;
			size_t MaxResourceSize;
			size_t m_NumResourcesToLoad;
		} Bootup;

		struct
		{
			float LoadInterval;
			size_t MinResourceSize;
			size_t MaxResourceSize;
			size_t MaxAllocatedResourceSize;
			float RunLength;
		} Gameplay;
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
		m_NextSeed(0),
		GameplayData({0.f, 0u})
	{
	}
	
	~ResourceLoadingScenario() {}

	// Functions
	void Run() override;
	void Reset() override;
	bool IsComplete() override { return m_bComplete; }
	
	void SetType(Type a_type) { m_CurrentType = a_type;  }
	
private:
	class IDummyResource
	{
	public:
		virtual ~IDummyResource() {}
		
		virtual size_t GetSize() = 0;
	};

	class DummyResource : public IDummyResource
	{
	public:
		DummyResource(size_t a_size)
			:
			m_Size(a_size),
			m_pData(nullptr)
		{
			m_pData = new char[a_size];
		}

		virtual ~DummyResource()
		{
			delete[] m_pData;
		}

		size_t GetSize() override { return m_Size; }

	private:
		size_t m_Size;
		char* m_pData;
	};
	
	void RunBootType();

	void RunGameplayType();

	bool m_bComplete;
	Type m_CurrentType;
	Config m_Config;

	Timer m_IntervalTimer;
	Timer m_RunTimeTimer;
	

	int m_NextSeed;

	std::vector<IDummyResource*> m_LoadedResources;

	struct
	{
		float m_TotalRunTime;
		size_t m_TotalLoadedResourceSize;
	} GameplayData;
	
};