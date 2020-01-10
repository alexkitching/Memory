#include "VertexDataProcessingScenario.h"
#include "RandomUtility.h"

VertexDataProcessingScenario::VertexDataProcessingScenario(const Config& a_config)
	:
m_Config(a_config),
m_bComplete(false)
{
}

void VertexDataProcessingScenario::Run()
{
	if(m_runTimer.IsStarted() == false)
	{
		m_runTimer.Start();
	}
	
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

	Vertex* pVerts = new Vertex[VertCount];
	
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

		delete[] sub.pVerts;

		m_Subs.pop_back();
	}
	m_fCurrentFrameDataSize = 0u;
}
