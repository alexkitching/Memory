#include "VertexDataProcessingScenario.h"
#include "RandomUtility.h"
#include "MemoryManager.h"
#include "MemSys.h"
#include "Common.h"
#include "Profiler.h"

VertexDataProcessingScenario::Config VertexDataProcessingScenario::Configuration = {};

VertexDataProcessingScenario::VertexDataProcessingScenario()
#if USE_MEM_SYS
	:
m_VertexAllocator(Configuration.PerFrameTotalData + (size_t)(0.5 * MB),
		MemoryManager::GetDefaultHeap()->allocate(Configuration.PerFrameTotalData + (size_t)(0.5 * MB)))
#endif
{
}

void VertexDataProcessingScenario::Initialise()
{
}

void VertexDataProcessingScenario::Run()
{
	PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::Run);
	
	ClearFrameSubs();
	
	while(m_fCurrentFrameDataSize < Configuration.PerFrameTotalData)
	{
		AddRandomSub();
	}

	PROFILER_END_SAMPLE();
}

void VertexDataProcessingScenario::Reset()
{
	PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::Run);
	ClearFrameSubs();
	PROFILER_END_SAMPLE();
}

void VertexDataProcessingScenario::AddRandomSub()
{
	PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::AddRandomSub);
	const int VertCount = Random::IntRange(Configuration.MinVertsPerSub, Configuration.MaxVertsPerSub);

#if USE_MEM_SYS
	Vertex* pVerts = (Vertex*)m_VertexAllocator.allocate(sizeof(Vertex) * VertCount, 4u);
#else
	Vertex* pVerts = new Vertex[VertCount];
#endif
	
	const VertexSub sub
	{
		pVerts,
		VertCount,
		{0}
	};

	m_fCurrentFrameDataSize += sizeof(Vertex) * VertCount;
	
	m_Subs.push_back(sub);
	PROFILER_END_SAMPLE();
}

void VertexDataProcessingScenario::ClearFrameSubs()
{
	PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::ClearFrameSubs);
	while(m_Subs.empty() == false)
	{
		VertexSub sub = m_Subs[m_Subs.size() - 1];
#if !USE_MEM_SYS
		delete[] sub.pVerts;
#endif
		m_Subs.pop_back();
	}

#if USE_MEM_SYS
	m_VertexAllocator.Clear();
#endif
	
	m_fCurrentFrameDataSize = 0u;

	PROFILER_END_SAMPLE();
}
