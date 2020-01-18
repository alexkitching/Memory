#pragma once

#include "Timer.h"
#include "Debug.h"
#include "Event.h"
#include "Delegate.h"
#include <vector>
#include <string>
#include <chrono>

#define PROFILER_ENABLED 1

#if PROFILER_ENABLED


class Profiler
{
public:
	struct SampleData
	{
		SampleData()
		{
			Depth = 0;
			TimeTaken = 0;
			Calls = 0;
		}
		std::string Name;
		int Depth;
		float TimeTaken;
		int Calls;
	};
	
	struct FrameData
	{
		int Frame;
		float TotalTimeTaken;
		float ProfilerOverhead;
		std::vector<SampleData> SampleData;
	};
	
	DECLARE_EVENT_ONE_PARAM(ProfilerSamplerEvent, void, void);
	
	static void Initialise();
	
	static void BeginSample(const char* a_pName);
	static void EndSample();

	static void OnFrameStart();
	static void OnFrameEnd();

	static void RecordNextFrame();

	static ProfilerSamplerEvent* OnSampleRecorded();
	static const FrameData& GetCurrentFrameData();

private:
	
	struct Sample
	{
		static Sample Create(const char* a_pName)
		{
			Sample newSample;
			newSample.Name = a_pName;
			newSample.StartTime = std::chrono::high_resolution_clock::now();
			return newSample;
		}
		
		std::string Name;
		std::chrono::high_resolution_clock::time_point StartTime;
	};

	
	struct SampleScope
	{
		SampleScope()
		{
			Depth = 0;
			TimeTaken = 0.f;
			Calls = 0;
		}

		inline void ResetTimer()
		{
			Sample.StartTime = std::chrono::high_resolution_clock::now();
		}

		inline void RecordTimer()
		{
			const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - Sample.StartTime;
			TimeTaken += duration.count();
		}
		
		Sample Sample;
		int Depth;
		float TimeTaken;
		int Calls;

		std::vector<SampleData> ChildData;
	};

	

	Profiler();

	void BeginSampleInternal(const char* a_pName);

	void EndSampleInternal();

	void OnFrameStartInternal();
	void OnFrameEndInternal();

	bool IsRecording() const { return m_bRecordNext || m_bRecording; }

	inline void BeginOverheadTimer();
	inline void StopOverheadTimer();

	inline void RecordCurrentStackTimes();
	inline void ResetCurrentStackTimes();

	static Profiler* s_pInstance;

	bool m_bRecording;
	bool m_bRecordNextRequested;
	bool m_bRecordNext;

	int m_CurrentFrame;
	float m_OverheadTime;
	std::chrono::high_resolution_clock::time_point m_OverheadStartTime;
	std::chrono::high_resolution_clock::time_point m_FrameStartTime;

	std::vector<SampleScope> m_CurrentScope;
	std::vector<SampleScope> m_OldScopeStack;
	std::vector<SampleData> m_OldDataCache;

	// Recorded Frame Data
	FrameData m_CurrentFrameData;
	std::vector<FrameData> m_RecordedFrameData;

	ProfilerSamplerEvent m_SampleRecordedEvent;
};

#define PROFILER_ONLY_CUSTOM 0

#if PROFILER_ONLY_CUSTOM
#define PROFILER_BEGIN_SAMPLE(name) 
#define PROFILER_END_SAMPLE() 
#define PROFILER_BEGIN_CUSTOMSAMPLE(name) Profiler::BeginSample(#name)
#define PROFILER_END_CUSTOMSAMPLE() Profiler::EndSample()
#else
#define PROFILER_BEGIN_SAMPLE(name) Profiler::BeginSample(#name)
#define PROFILER_END_SAMPLE() Profiler::EndSample()
#define PROFILER_BEGIN_CUSTOMSAMPLE(name)
#define PROFILER_END_CUSTOMSAMPLE() 
#endif


#else
#define PROFILER_BEGIN_SAMPLE(name)
#define PROFILER_END_SAMPLE()
#endif
