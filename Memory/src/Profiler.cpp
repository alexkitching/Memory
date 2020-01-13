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
	m_SampleStack.push_back(Sample::Create(a_pName));

	// Push New Scope
	SampleScope newScope;
	newScope.Parent = m_SampleStack.back();
	m_CurrentScope.push_back(newScope);
}

void Profiler::EndSampleInternal()
{
	ASSERT(m_SampleStack.empty() == false && "No Samples Started!");

	int size = (int)m_SampleStack.size();

	Sample* pSample = m_SampleStack[(size - 1)];

	int Depth = size - 1;
	// Record Data
	SampleData data;
	data.Depth = Depth;
	data.Frame = m_CurrentFrame;
	data.Name = pSample->Name;
	data.Calls = 1;

	// Record Duration
	const std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - pSample->StartTime;
	data.TimeTaken = duration.count();

	if(data.TimeTaken < DBL_EPSILON)
	{
		int n = 1;
	}

	// Get Parent Depth
	SampleScope* pCurrent = &m_CurrentScope.back();
	if (pCurrent->Parent == pSample)
	{
		if (m_CurrentScope.size() > 1) // At least another Scope
		{
			SampleScope* pParent = &m_CurrentScope[m_CurrentScope.size() - 2];

			SampleIdentifier id;
			id.Name = data.Name;

			auto pMatch  = pParent->ChildSet.find(id);
			
			if(pMatch != pParent->ChildSet.end())
			{
				int idx = pMatch->Index;
				SampleData* pData = &pParent->ChildData[idx];
				pData->Calls++;
				pData->TimeTaken += data.TimeTaken;
			}
			else
			{
				pParent->ChildData.push_back(data);
				id.Index = (int)pParent->ChildData.size() - 1;
				pParent->ChildSet.insert(id);
			}
			
			

			for (int i = 0; i < pCurrent->ChildData.size(); ++i)
			{
				pParent->ChildData.push_back(pCurrent->ChildData[i]);
			}
		}
		else
		{
			m_CurrentFrameData.SampleData.push_back(data);
			
			// Push Data to Current Frame Data
			for (int i = 0; i < pCurrent->ChildData.size(); ++i)
			{
				m_CurrentFrameData.SampleData.push_back(pCurrent->ChildData[i]);
			}
		}
		
		m_CurrentScope.pop_back();
	}

	// Pop Sampler from Stack
	delete pSample;
	m_SampleStack.pop_back();

	
}

void Profiler::OnFrameStartInternal()
{
	ASSERT(m_SampleStack.empty() && "Cannot Sample Before Frame Start!");
	m_FrameStartTime = std::chrono::high_resolution_clock::now();
}

void Profiler::OnFrameEndInternal()
{
	ASSERT(m_SampleStack.empty() && "Mismatched Samples");

	// Record Total Frame Time
	const std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - m_FrameStartTime;
	m_CurrentFrameData.TotalTimeTaken = duration.count();
	
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
