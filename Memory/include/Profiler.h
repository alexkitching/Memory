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
		int Frame;
		std::string Name;
		int Depth;
		double TimeTaken;
		int Calls;
	};
	
	struct FrameData
	{
		double TotalTimeTaken;
		std::vector<SampleData> SampleData;
	};
	
	DECLARE_EVENT_ONE_PARAM(ProfilerSamplerEvent, void, const FrameData&);
	
	static void Initialise();
	
	static void BeginSample(const char* a_pName);
	static void EndSample();

	static void OnFrameStart();
	static void OnFrameEnd();

	static void Pause() { s_bPaused = true; }
	static void Play() { s_bPaused = false; }

	static void RecordNextFrame();

	static ProfilerSamplerEvent* OnSampleRecorded();
	static const FrameData& GetCurrentFrameData();

private:
	
	struct Sample
	{
		static Sample* Create(const char* a_pName)
		{
			Sample* newSample = new Sample();
			newSample->Name = a_pName;
			newSample->StartTime = std::chrono::high_resolution_clock::now();
			return newSample;
		}
		std::string Name;
		std::chrono::high_resolution_clock::time_point StartTime;
	};

	struct SampleIdentifier
	{
		std::string Name;
		int Index;

		friend bool operator<(const SampleIdentifier& a_lhs, const SampleIdentifier& a_rhs)
		{
			return CompareStrings(a_lhs.Name, a_rhs.Name);
		}

	private:
		static bool icompare_pred(unsigned char a, unsigned char b)
		{
			return std::tolower(a) < std::tolower(b);
		}

		static bool CompareStrings(const std::string& a, const std::string& b)
		{
			return std::lexicographical_compare(a.begin(), a.end(),
				b.begin(), b.end(), icompare_pred);
		}
	};
	
	struct SampleScope
	{
		Sample* Parent;

		std::set<SampleIdentifier> ChildSet;
		std::vector<SampleData> ChildData;
	};

	

	Profiler();

	void BeginSampleInternal(const char* a_pName);

	void EndSampleInternal();

	void OnFrameStartInternal();
	void OnFrameEndInternal();

	

	static Profiler* s_pInstance;

	static bool s_bPaused;
	bool m_bRecording;
	bool m_bRecordNext;

	int m_CurrentFrame;
	std::chrono::high_resolution_clock::time_point m_FrameStartTime;

	std::vector<Sample*> m_SampleStack;
	std::vector<SampleScope> m_CurrentScope;
	FrameData m_CurrentFrameData;
	std::vector<FrameData> m_RecordedFrameData;

	ProfilerSamplerEvent m_SampleRecordedEvent;
	//static std::chrono::high_resolution_clock::time_point s_FrameStartTime;
};

#define PROFILER_BEGIN_SAMPLE(name) Profiler::BeginSample(#name)
#define PROFILER_END_SAMPLE() Profiler::EndSample()




#else
#define PROFILER_BEGIN_SAMPLE(name)
#define PROFILER_END_SAMPLE()
#endif
