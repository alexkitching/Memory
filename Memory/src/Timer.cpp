#include "Timer.h"


Timer::Timer(bool a_bStartImme)
	:
m_bStarted(false)
{
	if(a_bStartImme)
	{
		Start();
	}
}

void Timer::Start()
{
	if (m_bStarted)
		return;
	
	m_startTime = std::chrono::steady_clock::now();

	m_bStarted = true;
}

void Timer::Stop()
{
	m_bStarted = false;
	m_stopTime = std::chrono::steady_clock::now();;
}

float Timer::GetTime()
{
	if(m_bStarted)
	{
		const std::chrono::duration<float> time = std::chrono::steady_clock::now() - m_startTime;
		return time.count();
	}
	
	if(m_startTime != m_stopTime) // Stopped
	{
		const std::chrono::duration<float> time = m_stopTime - m_startTime;
		return time.count();
	}
	
	return 0.f;
}
