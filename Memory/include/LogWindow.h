#pragma once

#include "IMGUIInterface.h"
#include "IMGUIWindow.h"
#include "Debug.h"
#include <imgui.h>

class LogWindow : public IMGUIWindow, public ILogHandler
{
public:
	LogWindow()
		:
	IMGUIWindow(TEXT(LogWindow)),
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

	virtual void OnLog(const char* fmt, ...) IM_FMTARGS(2) override;

protected:
	void OnGUIWindow(const IMGUIInterface& a_interface) override;
private:
	ImGuiTextBuffer m_Buffer;
	ImGuiTextFilter m_Filter;

	ImVector<int> m_LineOffsets;
	bool m_bAutoScroll;
};