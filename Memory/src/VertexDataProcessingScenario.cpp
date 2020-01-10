#include "VertexDataProcessingScenario.h"
#include "RandomUtility.h"
#include "MemoryManager.h"

VertexDataProcessingScenario::VertexDataProcessingScenario(const Config& a_config)
	:
m_Config(a_config),
m_bComplete(false)
#if USE_MEM_SYS
,
m_VertexStack(a_config.PerFrameTotalData + (size_t)(0.5 * MB),
		MemoryManager::GetDefaultHeap()->allocate(a_config.PerFrameTotalData + (size_t)(0.5 * MB)))
#endif
{
}

void VertexDataProcessingScenario::Run()
{
	if(m_runTimer.IsStarted() == false)
	{
		m_runTimer.Start();
	}
	m_runTimer.Tick();
	
	ClearFrameSubs();
	
	while(m_fCurrentFrameDataSize < m_Config.PerFrameTotalData)
	{
		AddRandomSub();
	}

	if(m_runTimer.GetTime() > m_Config.RunLength)
	{
		m_bComplete = true;
	}
}

void VertexDataProcessingScenario::Reset()
{
	m_bComplete = false;
	ClearFrameSubs();
}

void VertexDataProcessingScenario::AddRandomSub()
{
	const int VertCount = Random::IntRange(m_Config.MinVertsPerSub, m_Config.MaxVertsPerSub);

#if USE_MEM_SYS
	Vertex* pVerts = (Vertex*)m_VertexStack.allocate(sizeof(Vertex) * VertCount, 4u);
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
}

void VertexDataProcessingScenario::ClearFrameSubs()
{
	while(m_Subs.empty() == false)
	{
		VertexSub sub = m_Subs[m_Subs.size() - 1];
#if USE_MEM_SYS
		m_VertexStack.deallocate(sub.pVerts);
#else
		delete[] sub.pVerts;
#endif
		m_Subs.pop_back();
	}
	
	m_fCurrentFrameDataSize = 0u;
}
