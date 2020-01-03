#pragma once

#include "Exception.h"

class AppBase
{
private:
	class AppException : public Exception
	{
	public:
		AppException(int a_line, const char* a_file, const char* a_ErrorString);

		const char* what() const override;

		virtual const char* GetType() const override { return TEXT(AppException); }
		std::string GetErrorString() const { return m_errorString; }

	private:
		std::string m_errorString;
	};
	
public:
	AppBase();
	virtual ~AppBase() {}

	virtual int Run();

protected:
	virtual void SetShouldClose(int a_exitCode) { m_bShouldClose = true; m_ExitCode = a_exitCode; }
	
	virtual bool Initialise() = 0;
	virtual void OnPreFrame() = 0;
	virtual void OnFrame() = 0;
	virtual void OnPostFrame() = 0;
	virtual void OnExit() = 0;
private:
	enum class FrameStage
	{
		PreFrame = 0,
		OnFrame,
		PostFrame,
		MAX
	};

	void CycleFrame();

	FrameStage m_FrameStage;
	bool m_bShouldClose;
	int m_ExitCode;
	//Window m_window;
};

#define APP_Except(errorStr) AppBase::AppException(__LINE__, __FILE__, errorStr)