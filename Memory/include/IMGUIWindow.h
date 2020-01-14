#pragma once

#include "IMGUIInterface.h"

class IMGUIWindow
{
public:
	IMGUIWindow(const char* a_pName, bool a_bStartOpen = true) : m_bOpen(a_bStartOpen), m_Name(a_pName) {};
	virtual ~IMGUIWindow() {}

	void OnGUI(const IMGUIInterface& a_interface);

	bool IsOpen() const { return m_bOpen; }
	void Open() { if (m_bOpen != true) { m_bOpen = true; OnOpen(); } }
	void Close() { if (m_bOpen != false) { m_bOpen = false; OnClose(); } }

protected:
	virtual void OnGUIWindow(const IMGUIInterface& a_gui) = 0;
	virtual void OnOpen() {}
	virtual void OnClose() {}

	bool m_bOpen;
private:
	std::string m_Name;
};