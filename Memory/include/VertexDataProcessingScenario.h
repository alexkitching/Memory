#pragma once
#include "Scenario.h"
#include "LinearAllocator.h"
#include "MemSys.h"
#include <vector>

//------------
// Description
//--------------
// Vertex Data Processing Memory Scenario
// Super High Frequency Per Frame Allocations and subsequent deallocations with a lifetime of a single frame demand fast allocator
// Using the custom memory system a linear allocator is used to allocate the vertex data.
//------------

class VertexDataProcessingScenario : public IScenario
{
public:
	struct Config
	{
		size_t PerFrameTotalData;
		int MaxVertsPerSub;
		int MinVertsPerSub;
	};
	static Config Configuration;

	VertexDataProcessingScenario();
	virtual ~VertexDataProcessingScenario() {}

	// IScenario Interface
	void Initialise() override;
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
		const Vertex* pVerts;
		int Count;
		char DummyExtraData[8];
	};

	// Adds a new Vertex Submission
	inline void AddSub(uint32 a_VertCount);
	void ClearFrameSubs();

	size_t m_fCurrentFrameDataSize;

#if USE_MEM_SYS
	LinearAllocator m_VertexAllocator;
#endif

	std::vector<VertexSub> m_Subs;
};
