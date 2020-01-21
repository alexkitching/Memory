#pragma once

#include "IMGUIInterface.h"

//------------
// Description
//--------------
// Base IMGUI Window Wrapper Class, reduces work needed to create a IMGUI Window
//------------

class IMGUIWindow
{
public:
	IMGUIWindow(const char* a_pName, bool a_bClosable, bool a_bStartOpen = true) : m_bOpen(a_bStartOpen), m_bClosable(a_bClosable), m_Name(a_pName) {};
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

	static void DrawQuad(const IMGUIQuad& a_quad);
private:
	
	bool m_bClosable; // Can this window be closed?
	std::string m_Name;
};
