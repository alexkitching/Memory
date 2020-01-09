#include "Timer.h"
#include "GlobalTime.h"


TP_Timer::TP_Timer(bool a_bStartImme)
	:
m_bStarted(false)
{
	if(a_bStartImme)
	{
		Start();
	}
}

void TP_Timer::Start()
{
	if (m_bStarted)
		return;

	m_startTime = std::chrono::high_resolution_clock::now();

	m_bStarted = true;
}

void TP_Timer::Stop()
{
	m_bStarted = false;
	m_stopTime = std::chrono::high_resolution_clock::now();;
}

float TP_Timer::GetTime() const
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

DT_Timer::DT_Timer(bool a_bStartImme)
	:
m_bStarted(false)
{
	if (a_bStartImme)
	{
		Start();
	}
}

void DT_Timer::Start()
{
	if (m_bStarted)
		return;

	m_CurrentTime = 0.f;
	m_bStarted = true;
}

void DT_Timer::Stop()
{
	m_bStarted = false;
}

void DT_Timer::Tick()
{
	if(m_bStarted)
	{
		m_CurrentTime += Time::DeltaTime();
	}
}
