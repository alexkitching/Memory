#include "Profiler.h"

#define OVERHEAD_START RecordCurrentStackTimes(); BeginOverheadTimer
#define OVERHEAD_END StopOverheadTimer(); ResetCurrentStackTimes

Profiler* Profiler::s_pInstance = nullptr;
 
void Profiler::Initialise()
{
	ASSERT(s_pInstance == nullptr && "Already Initialised");

	s_pInstance = new Profiler();
}

void Profiler::Shutdown()
{
	delete s_pInstance;
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
m_bRecordNextRequested(false),
m_bRecordNext(false),
m_CurrentFrame(1),
m_OverheadTime(0.f)
{
}

void Profiler::BeginSampleInternal(const char* a_pName)
{
	OVERHEAD_START();
	
	if(m_vOldScopeStack.empty() == false) // Previous Scope Exists
	{
		// Get Top
		SampleScope* pTop = &m_vOldScopeStack[static_cast<int>(m_vOldScopeStack.size()) - 1];
		if(pTop->Sample.Name == a_pName) // Reuse Scope / Same Call as This
		{
			// Push back onto Current from Old
			m_vCurrentScope.push_back(*pTop);
			m_vOldScopeStack.pop_back();

			OVERHEAD_END();
			return;
		}

		// New Scope - Push Old Scope Data
		const bool bCurrentEmpty = m_vCurrentScope.empty();
		while (m_vOldScopeStack.empty() == false) 
		{
			SampleScope* pScope = &m_vOldScopeStack.back();
			
			SampleData* pData = nullptr;
			std::vector<SampleData>* pDataContainer = nullptr;
			if(bCurrentEmpty == false) // Current Scope not empty, we will push to parent data
			{
				pDataContainer = &m_vCurrentScope.back().ChildData;
			}
			else // We will push directly to frame data
			{
				pDataContainer = &m_CurrentFrameData.SampleData;
			}
			
			// Push Data
			pData = &pDataContainer->emplace_back();
			pData->Name = pScope->Sample.Name;
			pData->Depth = pScope->Depth;
			pData->TimeTaken = pScope->TimeTaken;
			pData->Calls = pScope->Calls;

			const unsigned int Count = static_cast<unsigned int>(pScope->ChildData.size());
			for (unsigned int i = 0; i < Count; ++i)
			{
				pDataContainer->emplace_back(pScope->ChildData[i]);
			}
				
			m_vOldScopeStack.pop_back();
		}
	}
	
	// Push New Scope
	m_vCurrentScope.emplace_back(a_pName, static_cast<int>(m_vCurrentScope.size()));
	
	OVERHEAD_END();
}

void Profiler::EndSampleInternal()
{
	ASSERT(m_vCurrentScope.empty() == false && "No Samples Started!");
	OVERHEAD_START();

	// Get Current Scope
	SampleScope* pCurrent = &m_vCurrentScope.back();
	
	pCurrent->Calls++; // Increment Calls

	if (m_vCurrentScope.size() == 1) // At least another Scope
	{
		// Push Head
		SampleData* data = &m_CurrentFrameData.SampleData.emplace_back(SampleData());
		data->Name = pCurrent->Sample.Name;
		data->Depth = pCurrent->Depth;
		data->TimeTaken = pCurrent->TimeTaken;
		data->Calls = pCurrent->Calls;

		// Push Mid Child Data
		for(int i = 0; i < (int)pCurrent->ChildData.size(); ++i)
		{
			m_CurrentFrameData.SampleData.emplace_back(pCurrent->ChildData[i]);
		}

		// Push Tail
		while(m_vOldScopeStack.empty() == false)
		{
			SampleScope* pScope = &m_vOldScopeStack.back();
			
			data = &m_CurrentFrameData.SampleData.emplace_back(SampleData());
			data->Name = pScope->Sample.Name;
			data->Depth = pScope->Depth;
			data->TimeTaken = pScope->TimeTaken;
			data->Calls = pScope->Calls;
			
			for (int i = 0; i < (int)pScope->ChildData.size(); ++i)
			{
				m_CurrentFrameData.SampleData.emplace_back(pScope->ChildData[i]);
			}
			
			m_vOldScopeStack.pop_back();
		}
	}
	else // Move Current to Old
	{
		m_vOldScopeStack.emplace_back(m_vCurrentScope.back());
	}
	
	m_vCurrentScope.pop_back();
	
	OVERHEAD_END();
}

void Profiler::OnFrameStartInternal()
{
	ASSERT(m_vCurrentScope.empty() && "Cannot Sample Before Frame Start!");
	m_FrameStartTime = std::chrono::high_resolution_clock::now();
}

void Profiler::OnFrameEndInternal()
{
	ASSERT(m_vCurrentScope.empty() && "Mismatched Samples");

	// Record Total Frame Time
	const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - m_FrameStartTime;
	m_CurrentFrameData.TotalTimeTaken = duration.count();
	m_CurrentFrameData.ProfilerOverhead = m_OverheadTime;
	m_CurrentFrameData.Frame = m_CurrentFrame;
	
	if (IsRecording())
	{
		// Record This Frames Data
		//m_vRecordedFrameData.push_back(m_CurrentFrameData); // Not Supporting More than single frame

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
	const unsigned int Count = static_cast<unsigned int>(m_vCurrentScope.size());
	for(unsigned int i = 0; i < Count; ++i)
	{
		m_vCurrentScope[i].RecordTimer();
	}
}

void Profiler::ResetCurrentStackTimes()
{
	const unsigned int Count = static_cast<unsigned int>(m_vCurrentScope.size());
	for (unsigned int i = 0; i < Count; ++i)
	{
		m_vCurrentScope[i].ResetTimer();
	}
}
