#pragma once
#include "Scenario.h"
#include "Timer.h"
#include "LinearAllocator.h"
#include <vector>



class VertexDataProcessingScenario : public IScenario
{
public:
	struct Config
	{
		size_t PerFrameTotalData;
		int MaxVertsPerSub;
		int MinVertsPerSub;
	};

	VertexDataProcessingScenario() = default;
	VertexDataProcessingScenario(const Config& a_config);

	virtual ~VertexDataProcessingScenario() {}

	void Run() override;
	
	void Reset() override;
	bool IsComplete() override { return false; }

	

private:
	struct Vertex
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct VertexSub
	{
		Vertex* pVerts;
		int Count;
		char DummyExtraData[8];
	};

	void AddRandomSub();
	void ClearFrameSubs();

	Config m_Config;

	size_t m_fCurrentFrameDataSize;

#if USE_MEM_SYS
	LinearAllocator m_VertexAllocator;
#endif

	std::vector<VertexSub> m_Subs;
};
