#pragma once

#include "Window.h"
#include "App.h"
#include "GlobalTime.h"

//------------
// Description
//--------------
// Base Windows Application Class, Single DX11 Rendered Window via Window instance and D3DRenderer.
//------------

// Forward Declarations
class Time;
class WinApp : public IApp
{
public:
	WinApp();
	virtual ~WinApp(){}

	Window& GetWindow() const;

	// Called from Main
	virtual int Run() override final;
protected:
	virtual bool Initialise() override;

	// Rendered Frame Stages
	virtual void OnPreFrame();
	virtual void OnFrame();
	// Render Frame
	virtual void OnPreRenderFrame();
	virtual void OnRenderFrame(IRenderer* a_pRenderer);
	virtual void OnPostRenderFrame();
	virtual void OnPostFrame();

	virtual void OnExit() override;

	// IMGUI Call Func
	virtual void OnGUI(IMGUIInterface& a_imGUIInterface) {}

	void SetShouldClose(int a_exitCode) { m_bShouldClose = true; m_iExitCode = a_exitCode; }
	
private:
	enum class FrameStage
	{
		PreFrame = 0,
		OnFrame,
		OnRenderFrame,
		PostFrame,
		MAX
	};

	virtual void CycleFrame();

	
	Window* m_pWindow;
	BOOL m_Result;
	Time m_Time; // Global Time Instance

	FrameStage m_FrameStage;
	bool m_bShouldClose;
	int m_iExitCode;
};
