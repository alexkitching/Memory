#pragma once
#include <chrono>

class TP_Timer
{
public:
	TP_Timer(bool a_bStartImme = false);

	void Start();
	void Stop();

	bool IsStarted() const { return m_bStarted; }

	float GetTime() const;
private:
	std::chrono::high_resolution_clock::time_point m_startTime;
	std::chrono::high_resolution_clock::time_point m_stopTime;
	bool m_bStarted;
};

class DT_Timer
{
public:
	DT_Timer(bool a_bStartImme = false);

	void Start();
	void Stop();
	void Tick();

	bool IsStarted() const { return m_bStarted; }

	float GetTime() const { return m_CurrentTime; }
	void SetTime(float a_fTime) { m_CurrentTime = a_fTime; }
private:
	float m_CurrentTime;
	bool m_bStarted;
};