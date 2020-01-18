#include "Profiler.h"

Profiler* Profiler::s_pInstance = nullptr;
 
void Profiler::Initialise()
{
	ASSERT(s_pInstance == nullptr && "Already Initialised");

	s_pInstance = new Profiler();
}

void Profiler::BeginSample(const char* a_pName)
{
	ASSERT(s_pInstance != nullptr);
	
	if (s_pInstance->IsRecording() == false)
		return;
	
	s_pInstance->BeginSampleInternal(a_pName);
}

void Profiler::EndSample()
{
	ASSERT(s_pInstance != nullptr);

	if (s_pInstance->IsRecording() == false)
		return;
	
	s_pInstance->EndSampleInternal();
}

void Profiler::OnFrameStart()
{
	ASSERT(s_pInstance != nullptr);
	s_pInstance->OnFrameStartInternal();
}

void Profiler::OnFrameEnd()
{
	ASSERT(s_pInstance != nullptr);
	s_pInstance->OnFrameEndInternal();
}

void Profiler::RecordNextFrame()
{
	ASSERT(s_pInstance != nullptr);
	s_pInstance->m_bRecordNextRequested = true;
}

Profiler::ProfilerSamplerEvent* Profiler::OnSampleRecorded()
{
	ASSERT(s_pInstance != nullptr);

	return &s_pInstance->m_SampleRecordedEvent;
}

const Profiler::FrameData& Profiler::GetCurrentFrameData()
{
	ASSERT(s_pInstance != nullptr);

	return s_pInstance->m_CurrentFrameData;
}

Profiler::Profiler()
:
m_bRecording(false),
m_bRecordNext(false),
m_CurrentFrame(1)
{
}

void Profiler::BeginSampleInternal(const char* a_pName)
{
	RecordCurrentStackTimes();
	BeginOverheadTimer();
	
	if(m_OldScopeStack.empty() == false)
	{
		SampleScope* pTop = &m_OldScopeStack[(int)m_OldScopeStack.size() - 1];
		if(pTop != nullptr)
		{
			if(pTop->Sample.Name == a_pName) // Reuse Scope
			{
				m_CurrentScope.push_back(*pTop);
				m_OldScopeStack.pop_back();

				StopOverheadTimer();
				ResetCurrentStackTimes();
				return;
			}
			
			SampleData data;
			while (m_OldScopeStack.empty() == false)
			{
				SampleScope* pScope = &m_OldScopeStack.back();
				data.Name = pScope->Sample.Name;
				data.Depth = pScope->Depth;
				data.TimeTaken = pScope->TimeTaken;
				data.Calls = pScope->Calls;

				std::vector<SampleData>* pDataContainer = nullptr;
				if(m_CurrentScope.empty() == false)
				{
					pDataContainer = &m_CurrentScope.back().ChildData;
				}
				else
				{
					pDataContainer = &m_CurrentFrameData.SampleData;
				}

				if(pDataContainer != nullptr)
				{
					pDataContainer->push_back(data);
					for (int i = 0; i < (int)pScope->ChildData.size(); ++i)
					{
						pDataContainer->push_back(pScope->ChildData[i]);
					}
				}
					
				m_OldScopeStack.pop_back();
			}
		}
	}
	
	// Push New Scope
	SampleScope newScope;
	newScope.Sample = Sample::Create(a_pName);
	newScope.Depth = (int)m_CurrentScope.size();
	m_CurrentScope.push_back(newScope);
	
	StopOverheadTimer();
	ResetCurrentStackTimes();
}

void Profiler::EndSampleInternal()
{
	ASSERT(m_CurrentScope.empty() == false && "No Samples Started!");
	RecordCurrentStackTimes();
	BeginOverheadTimer();

	// Get Current Scope
	SampleScope* pCurrent = &m_CurrentScope.back();
	
	pCurrent->Calls++;

	if (m_CurrentScope.size() == 1) // At least another Scope
	{
		SampleData data;
		data.Name = pCurrent->Sample.Name;
		data.Depth = pCurrent->Depth;
		data.TimeTaken = pCurrent->TimeTaken;
		data.Calls = pCurrent->Calls;

		// Push Head
		m_CurrentFrameData.SampleData.push_back(data);

		// Push Mid Child Data
		for(int i = 0; i < (int)pCurrent->ChildData.size(); ++i)
		{
			m_CurrentFrameData.SampleData.push_back(pCurrent->ChildData[i]);
		}

		// Push Tail
		while(m_OldScopeStack.empty() == false)
		{
			SampleScope* pScope = &m_OldScopeStack.back();
			data.Name = pScope->Sample.Name;
			data.Depth = pScope->Depth;
			data.TimeTaken = pScope->TimeTaken;
			data.Calls = pScope->Calls;
			m_CurrentFrameData.SampleData.push_back(data);
			
			for (int i = 0; i < (int)pScope->ChildData.size(); ++i)
			{
				m_CurrentFrameData.SampleData.push_back(pScope->ChildData[i]);
			}
			
			m_OldScopeStack.pop_back();
		}
	}
	else
	{
		m_OldScopeStack.push_back(m_CurrentScope.back());
	}

	
	m_CurrentScope.pop_back();
	
	StopOverheadTimer();
	ResetCurrentStackTimes();
}

void Profiler::OnFrameStartInternal()
{
	ASSERT(m_CurrentScope.empty() && "Cannot Sample Before Frame Start!");
	m_FrameStartTime = std::chrono::high_resolution_clock::now();
}

void Profiler::OnFrameEndInternal()
{
	ASSERT(m_CurrentScope.empty() && "Mismatched Samples");

	// Record Total Frame Time
	const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - m_FrameStartTime;
	m_CurrentFrameData.TotalTimeTaken = duration.count();
	m_CurrentFrameData.ProfilerOverhead = m_OverheadTime;
	m_CurrentFrameData.Frame = m_CurrentFrame;
	
	if (IsRecording())
	{
		// Record This Frames Data
		m_RecordedFrameData.push_back(m_CurrentFrameData);

		m_SampleRecordedEvent.Raise();
		
		m_bRecordNext = false;
		m_bRecordNextRequested = false;
	}
	
	// Clear Current Frame Data
	m_OverheadTime = 0.f;
	m_CurrentFrameData.SampleData.clear();

	m_CurrentFrame++;

	if(m_bRecordNextRequested)
	{
		m_bRecordNext = true;
	}
}

void Profiler::BeginOverheadTimer()
{
	m_OverheadStartTime = std::chrono::high_resolution_clock::now();
}

void Profiler::StopOverheadTimer()
{
	const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - m_OverheadStartTime;
	m_OverheadTime += duration.count();
}

void Profiler::RecordCurrentStackTimes()
{
	for(int i = 0; i < m_CurrentScope.size(); ++i)
	{
		m_CurrentScope[i].RecordTimer();
	}
}

void Profiler::ResetCurrentStackTimes()
{
	for (int i = 0; i < m_CurrentScope.size(); ++i)
	{
		m_CurrentScope[i].ResetTimer();
	}
}
