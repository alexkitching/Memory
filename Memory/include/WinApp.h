#pragma once

#include "Window.h"
#include "App.h"
#include "GlobalTime.h"

class Time;
class WinApp : public IApp
{
public:
	WinApp();
	virtual ~WinApp(){}

	Window& GetWindow() const;

	virtual int Run() override final;
protected:
	virtual bool Initialise() override;
	virtual void OnPreFrame();
	virtual void OnFrame();
	virtual void OnPreRenderFrame();
	virtual void OnRenderFrame(IRenderer* a_pRenderer);
	virtual void OnPostRenderFrame();
	virtual void OnGUI(IMGUIInterface& a_imGUIInterface) {}
	virtual void OnPostFrame();
	virtual void OnExit() override;

	void SetShouldClose(int a_exitCode) { m_bShouldClose = true; m_ExitCode = a_exitCode; }

	
private:
	enum class FrameStage
	{
		PreFrame = 0,
		OnFrame,
		OnRenderFrame,
		PostFrame,
		MAX
	};

	void CycleFrame();

	
	Window* m_pWindow;
	BOOL m_Result;
	Time m_Time;

	FrameStage m_FrameStage;
	bool m_bShouldClose;
	int m_ExitCode;
};
