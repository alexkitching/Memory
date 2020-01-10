#pragma once
#include "Scenario.h"
#include "Timer.h"
#include <vector>

class VertexDataProcessingScenario : public IScenario
{
public:
	struct Config
	{
		size_t PerFrameTotalData;
		int MaxVertsPerSub;
		int MinVertsPerSub;
		float RunLength;
	};

	VertexDataProcessingScenario() = default;
	VertexDataProcessingScenario(const Config& a_config);

	virtual ~VertexDataProcessingScenario() {}

	void Run() override;
	
	void Reset() override;
	bool IsComplete() override { return m_bComplete; }

	

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

	bool m_bComplete;
	DT_Timer m_runTimer;

	size_t m_fCurrentFrameDataSize;

	std::vector<VertexSub> m_Subs;
};
