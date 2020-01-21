#include "GlobalTime.h"
#include "Debug.h"

Time* Time::s_pInstance = nullptr;

Time::Time()
	:
m_deltaTime(0.f)
{
	ASSERT(s_pInstance == nullptr && "Time Instance Already Exists!");
	s_pInstance = this;
	m_startTime = std::chrono::steady_clock::now();
	m_lastFrameTime = m_startTime;
}

Time::~Time()
{
}

void Time::Update()
{
	// Update Delta Time
	const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	const std::chrono::duration<float> deltaTime = now - m_lastFrameTime;
	m_deltaTime = deltaTime.count();
	m_lastFrameTime = now;
}

float Time::TimeSinceStartup()
{
	const std::chrono::duration<float> time = std::chrono::steady_clock::now() - s_pInstance->m_startTime;
	return time.count();
}

float Time::DeltaTime()
{
	return s_pInstance->m_deltaTime;
}

float Time::CurrentFrameTime()
{
	const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	const std::chrono::duration<float> currentFrameTime = now - s_pInstance->m_lastFrameTime;
	return currentFrameTime.count();
}
