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
	Sample newSample = Sample::Create(a_pName);

	if(m_OldScopeStack.size() > 0)
	{
		SampleScope* pTop = &m_OldScopeStack[(int)m_OldScopeStack.size() - 1];
		if(pTop != nullptr)
		{
			if(pTop->Sample.Name == a_pName) // Reuse Scope
			{
				// Reset Sample Time
				pTop->Sample.Reset();
				m_CurrentScope.push_back(*pTop);
				m_OldScopeStack.pop_back();
				
				return;
			}
			
			SampleData data;
			while (m_OldScopeStack.size() > 0)
			{
				SampleScope* pScope = &m_OldScopeStack.back();
				data.Name = pScope->Sample.Name;
				data.Depth = pScope->Depth;
				data.TimeTaken = pScope->TimeTaken;
				data.Calls = pScope->Calls;

				std::vector<SampleData>* pDataContainer = nullptr;
				if(m_CurrentScope.size() > 0)
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
	newScope.Sample = newSample;
	newScope.Depth = (int)m_CurrentScope.size();
	m_CurrentScope.push_back(newScope);
}

void Profiler::EndSampleInternal()
{
	ASSERT(m_CurrentScope.empty() == false && "No Samples Started!");

	// Get Current Scope
	SampleScope* pCurrent = &m_CurrentScope.back();
	pCurrent->Calls++;

	// Record Duration
	const std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - pCurrent->Sample.StartTime;
	pCurrent->TimeTaken += duration.count();

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
		for(int i = 0; i < pCurrent->ChildData.size(); ++i)
		{
			m_CurrentFrameData.SampleData.push_back(pCurrent->ChildData[i]);
		}

		// Push Tail
		while(m_OldScopeStack.size() > 0)
		{
			SampleScope* pScope = &m_OldScopeStack.back();
			data.Name = pScope->Sample.Name;
			data.Depth = pScope->Depth;
			data.TimeTaken = pScope->TimeTaken;
			data.Calls = pScope->Calls;
			m_CurrentFrameData.SampleData.push_back(data);
			
			for (int i = 0; i < pScope->ChildData.size(); ++i)
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
	const std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - m_FrameStartTime;
	m_CurrentFrameData.TotalTimeTaken = duration.count();
	m_CurrentFrameData.Frame = m_CurrentFrame;
	
	if (IsRecording())
	{
		// Record This Frames Data
		m_RecordedFrameData.push_back(m_CurrentFrameData);

		m_SampleRecordedEvent.Raise(m_CurrentFrameData);
		
		m_bRecordNext = false;
		m_bRecordNextRequested = false;
	}
	
	// Clear Current Frame Data
	m_CurrentFrameData.SampleData.clear();

	m_CurrentFrame++;

	if(m_bRecordNextRequested)
	{
		m_bRecordNext = true;
	}
}
