#include "App.h"
#include <sstream>

AppBase::AppException::AppException(int a_line, const char* a_file, const char* a_ErrorString)
	:
Exception(a_line, a_file),
m_errorString(a_ErrorString)
{
}

const char* AppBase::AppException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginalString();

	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

AppBase::AppBase()
	:
m_FrameStage(FrameStage::PreFrame),
m_bShouldClose(false)
{
}

int AppBase::Run()
{
	m_bShouldClose = false;

	const bool bSuccess = Initialise();

	if(bSuccess == false)
	{
		throw APP_Except("Initialisation Failed");
	}

	while(m_bShouldClose == false)
	{
		CycleFrame();
	}

	OnExit();

	return m_ExitCode;
}

void AppBase::CycleFrame()
{
	switch (m_FrameStage)
	{
	case FrameStage::PreFrame:
		OnPreFrame();
		m_FrameStage = FrameStage::OnFrame;
		break;
	case FrameStage::OnFrame:
		OnFrame();
		m_FrameStage = FrameStage::PostFrame;
		break;
	case FrameStage::PostFrame:
		OnPostFrame();
		m_FrameStage = FrameStage::PreFrame;
		break;
	default:
		break;
	}
}
