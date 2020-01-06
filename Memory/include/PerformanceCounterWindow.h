#pragma once

#include "IMGUIWindow.h"

class PerformanceCounterWindow : public IMGUIWindow
{
public:
	PerformanceCounterWindow();
	virtual ~PerformanceCounterWindow();

protected:
	void OnGUIWindow(const IMGUIInterface& a_interface) override;


private:
	float m_fDeltaTime;
	float m_fAccumFPS;
	int m_FrameCount;
	const float m_fFPSUpdateRate = 2.f;
	float m_fFPS;
};