#pragma once
#include "Scenario.h"
#include "Timer.h"
#include "RandomUtility.h"
#include <vector>
#include "MoveableHeap.h"

#define USE_MOVEABLE_HEAP 1

class MoveableHeap;
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
			size_t TotalSizeToLoad;
		} Bootup;

		struct
		{
			float LoadInterval;
			size_t MinResourceSize;
			size_t MaxResourceSize;
			size_t AllocatedResourceCap;
		} Gameplay;
	};
	static Config Configuration;

	enum class Type
	{
		Invalid = -1,
		Bootup,
		Gameplay
	};

	// Constructors
	ResourceLoadingScenario()
		:
		m_bComplete(false),
		m_CurrentType(Type::Invalid),
		m_NextSeed(0),
		GameplayData({0.f})
	{
	}
	
	~ResourceLoadingScenario() {}

	// Functions
	void Initialise() override;
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
		DummyResource(size_t a_size);
		virtual ~DummyResource();

		size_t GetSize() override { return m_Size; }

#if USE_MOVEABLE_HEAP
		static MoveableHeap* s_pMoveableHeap;
#endif
	private:
		size_t m_Size;
#if USE_MOVEABLE_HEAP
		MoveablePointer<char> m_pData;
#else
		char* m_pData;
#endif
	};
	
	void RunBootType();

	void RunGameplayType();
	// Used by Both
	void LoadResource(size_t a_size);
	// Gameplay Only
	void FreeResource(int a_idx);

	bool m_bComplete;
	Type m_CurrentType;

	DT_Timer m_IntervalTimer;

	int m_NextSeed;
	
	std::vector<IDummyResource*> m_LoadedResources;
	size_t m_CurrentTotalLoadedSize;
	struct
	{
		float m_TotalRunTime;
	} GameplayData;
	
};