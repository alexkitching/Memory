#pragma once
#include <chrono>

//------------
// Description
//--------------
// Static Time Class used for global retrieval of common time data.
//------------

class Time
{
public:
	Time();
	~Time();

	void Update();

	static float TimeSinceStartup();
	static float DeltaTime();

	static float CurrentFrameTime();
private:
	static Time* s_pInstance;

	std::chrono::steady_clock::time_point m_startTime;
	std::chrono::steady_clock::time_point m_lastFrameTime;
	float m_deltaTime;
};
