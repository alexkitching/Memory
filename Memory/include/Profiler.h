#pragma once

#include "Timer.h"
#include "Debug.h"
#include "Event.h"
#include <vector>
#include <string>
#include <chrono>

//------------
// Description
//--------------
// Simple Profiler (Can be optimised more) but does the job for recording a single frames data accurately with consideration of overhead. 
//------------

#define PROFILER_ENABLED 1

#if PROFILER_ENABLED


class Profiler
{
public:
	// Data of a Single Sample
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

	// Frame data containing a collection of sample data with framestats.
	struct FrameData
	{
		FrameData()
		{
			Frame = 0u;
			TotalTimeTaken = 0.f;
			ProfilerOverhead = 0.f;
		}
		unsigned int Frame;
		float TotalTimeTaken;
		float ProfilerOverhead;
		std::vector<SampleData> SampleData;
	};

	// Declare Profiler Sample Event
	DECLARE_EVENT_ONE_PARAM(ProfilerSamplerEvent, void, void);

	// Base Static Interface Function
	static void Initialise();
	static void Shutdown();
	
	static void BeginSample(const char* a_pName);
	static void EndSample();

	static void OnFrameStart();
	static void OnFrameEnd();

	// Tells the profiler to record the next frame.
	static void RecordNextFrame();

	// Public Event for Subscribing to Sample Recorded 
	static ProfilerSamplerEvent* OnSampleRecorded();
	// Gets the Current Frame Data
	static const FrameData& GetCurrentFrameData();

private:

	// Live Sample Structure
	struct Sample
	{
		Sample(const char* a_pName)
		{
			Name = a_pName;
			StartTime = std::chrono::high_resolution_clock::now();
		}
		
		std::string Name;
		std::chrono::high_resolution_clock::time_point StartTime;
	};

	// Sample Scope Structure, Contains Sample Itself + Extra Info about Current Scope including Child Data
	struct SampleScope
	{
		SampleScope(const char* a_pName, int a_Depth)
			:
		Sample(Profiler::Sample(a_pName)),
		Depth(a_Depth),
		TimeTaken(0.f),
		Calls(0)
		{
		}

		inline void ResetTimer(const std::chrono::high_resolution_clock::time_point& a_currentTime)
		{
			Sample.StartTime = a_currentTime;
		}

		inline void RecordTimer(const std::chrono::high_resolution_clock::time_point& a_currentTime)
		{
			const std::chrono::duration<float> duration = a_currentTime - Sample.StartTime;
			TimeTaken += duration.count();
		}
		
		Sample Sample;
		int Depth;
		float TimeTaken;
		int Calls;

		std::vector<SampleData> ChildData;
	};

	Profiler();
	~Profiler() {}

	Profiler(const Profiler&) = delete;
	Profiler& operator=(const Profiler&) = delete;

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

	// Overhead Tracking Data
	float m_OverheadTime;
	std::chrono::high_resolution_clock::time_point m_OverheadStartTime;
	std::chrono::high_resolution_clock::time_point m_FrameStartTime;

	std::vector<SampleScope> m_vCurrentScope;
	std::vector<SampleScope> m_vOldScopeStack;

	// Recorded Frame Data
	FrameData m_CurrentFrameData;
	std::vector<FrameData> m_vRecordedFrameData;

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
