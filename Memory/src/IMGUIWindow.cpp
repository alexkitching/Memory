#include "IMGUIWindow.h"
#include <imgui.h>
#include <imgui_internal.h>

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

void IMGUIWindow::DrawQuad(const IMGUIQuad& a_quad)
{
	const ImVec2 wndPos = ImGui::GetWindowPos();
	const ImVec2 relPos{ wndPos.x + a_quad.x, wndPos.y + a_quad.y };

	const ImRect imgRect(
		ImVec2(relPos.x - (a_quad.w * 0.5f), relPos.y - (a_quad.h * 0.5f)),
		ImVec2(relPos.x + (a_quad.w * 0.5f), relPos.y + (a_quad.h * 0.5f)));

	const ImVec2 p1 = { imgRect.Min };
	const ImVec2 p2 = { imgRect.Min.x, imgRect.Max.y };
	const ImVec2 p3 = { imgRect.Max };
	const ImVec2 p4 = { imgRect.Max.x, imgRect.Min.y };


	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->AddQuadFilled(p1, p2, p3, p4, ImColor{a_quad.r, a_quad.g, a_quad.b, a_quad.a});

}
