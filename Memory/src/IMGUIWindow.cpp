#include "IMGUIWindow.h"
#include <imgui.h>

void IMGUIWindow::OnGUI(const IMGUIInterface& a_interface)
{
	if(m_bOpen == false)
	{
		return;
	}

	bool* pCloseBool = nullptr;
	if(m_bClosable)
	{
		pCloseBool = &m_bOpen;
	}
	
	if (ImGui::Begin(m_Name.c_str(), pCloseBool) == false)
	{
		ImGui::End();
		return;
	}

	OnGUIWindow(a_interface);

	ImGui::End();
}
