#pragma once
#include <chrono>

class Timer
{
public:
	Timer(bool a_bStartImme = true);

	void Start();
	void Stop();

	bool IsStarted() const { return m_bStarted; }

	float GetTime() const;
private:
	std::chrono::steady_clock::time_point m_startTime;
	std::chrono::steady_clock::time_point m_stopTime;
	bool m_bStarted;
};