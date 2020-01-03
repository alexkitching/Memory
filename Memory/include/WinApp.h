#pragma once

#include "Window.h"
#include "App.h"
#include "Timer.h"


class WinApp : public AppBase
{
public:
	WinApp();
	virtual ~WinApp(){}

	Window& GetWindow() const;

	virtual int Run() override;
protected:
	virtual bool Initialise() override;
	virtual void OnPreFrame() override;
	virtual void OnFrame() override;
	virtual void OnPostFrame() override;
	virtual void OnExit() override;
private:
	Window* m_pWindow;
	BOOL m_Result;
	Timer m_Timer;
};