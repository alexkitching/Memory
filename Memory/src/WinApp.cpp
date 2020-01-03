#include "WinApp.h"
#include <sstream>
#include <iomanip>

#if DEBUG
#include "DXGI_Info_Man.h"
#endif

WinApp::WinApp()
	:
m_pWindow(nullptr),
m_Timer(false)
{
}

Window& WinApp::GetWindow() const
{
	return *m_pWindow;
}

int WinApp::Run()
{
	int result = 0;
	try
	{
		result = AppBase::Run();
	}
	catch (const Exception& e)
	{
		MessageBox(m_pWindow->GetHandle(), e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(m_pWindow->GetHandle(), e.what(), "std::Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(m_pWindow->GetHandle(), "No Details Available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return result;
}

bool WinApp::Initialise()
{
	m_pWindow = new Window(800, 600, "Memory");

#if DEBUG
	DXGIInfoManager::Initialise();
#endif
	
	m_pWindow->Initialise();

	m_Timer.Start();
	return m_pWindow != nullptr;
}

void WinApp::OnPreFrame()
{
	// Process Windows Messages
	if(const std::optional<int> errorCode = Window::ProcessMessages())
	{
		// Optional Has Value
		SetShouldClose(*errorCode);
	}
}

void WinApp::OnFrame()
{
	const float t = m_Timer.GetTime();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	m_pWindow->SetTitle(oss.str());
}

void WinApp::OnPostFrame()
{
	float c = sin(m_Timer.GetTime()) / 2.f + 0.5f;
	m_pWindow->GetRenderer().Clear(c, c, 1.0f);
	m_pWindow->GetRenderer().EndFrame();
}

void WinApp::OnExit()
{
#if DEBUG
	DXGIInfoManager::Shutdown();
#endif
}
