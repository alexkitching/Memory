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
};