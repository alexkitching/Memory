#pragma once

#include "IMGUIInterface.h"
#include "IMGUIWindow.h"
#include "Debug.h"
#include <imgui.h>

//------------
// Description
//--------------
// Log Window Based on IMGUIs example Implementation, Hooked up to Debug Static class as the log handler.
//------------

class LogWindow : public IMGUIWindow, public ILogHandler
{
public:
	LogWindow()
		:
	IMGUIWindow(TEXT(LogWindow), false),
	m_bAutoScroll(true)
	{
		Debug::SetLogHandler(this);
		Clear();
	}
	virtual ~LogWindow() {}

	void Clear()
	{
		m_Buffer.clear();
		m_LineOffsets.clear();
		m_LineOffsets.push_back(0);
	}

	virtual void Log(const char* fmt, ...) override;
	virtual void OnLog(const char* fmt, va_list a_va) IM_FMTARGS(2) override;

protected:
	void OnGUIWindow(const IMGUIInterface& a_interface) override;
private:
	ImGuiTextBuffer m_Buffer;
	ImGuiTextFilter m_Filter;

	ImVector<int> m_LineOffsets;
	bool m_bAutoScroll;
};