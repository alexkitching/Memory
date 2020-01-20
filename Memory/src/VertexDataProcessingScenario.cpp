#include "VertexDataProcessingScenario.h"
#include "RandomUtility.h"
#include "MemoryManager.h"
#include "MemSys.h"
#include "Common.h"
#include "Profiler.h"

VertexDataProcessingScenario::Config VertexDataProcessingScenario::Configuration = {};

VertexDataProcessingScenario::VertexDataProcessingScenario()
#if USE_MEM_SYS

#endif
{
}

void VertexDataProcessingScenario::Initialise()
{
	m_VertexAllocator = LinearAllocator(Configuration.PerFrameTotalData + (size_t)(0.5 * MB),
		MemoryManager::GetDefaultHeap()->allocate(Configuration.PerFrameTotalData + (size_t)(0.5 * MB)));
}

void VertexDataProcessingScenario::Run()
{
	PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::Run);
	
	ClearFrameSubs();
	
	while(m_fCurrentFrameDataSize < Configuration.PerFrameTotalData)
	{
		AddSub(Random::IntRange(Configuration.MinVertsPerSub, Configuration.MaxVertsPerSub));
	}

	PROFILER_END_SAMPLE();
}

void VertexDataProcessingScenario::Reset()
{
	PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::Run);
	ClearFrameSubs();
	m_VertexAllocator.Clear();
	MemoryManager::GetDefaultHeap()->deallocate(m_VertexAllocator.GetStartAddress());
	PROFILER_END_SAMPLE();
}

void VertexDataProcessingScenario::AddSub(uint32 a_VertCount)
{
	PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::AddRandomSub);
	
	m_fCurrentFrameDataSize += sizeof(Vertex) * a_VertCount;
	
	VertexSub& NewSub = m_Subs.emplace_back();
	
PROFILER_BEGIN_SAMPLE(VertexDataProcessingScenario::AllocateVertices); //7ms
#if USE_MEM_SYS
	NewSub.pVerts = static_cast<Vertex*>(m_VertexAllocator.allocate(sizeof(Vertex) * a_VertCount, 4u));
#else
	NewSub.pVerts = new Vertex[a_VertCount];
#endif
PROFILER_END_SAMPLE();
	
	NewSub.Count = a_VertCount;
	
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
