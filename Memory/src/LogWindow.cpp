#include "LogWindow.h"


void LogWindow::OnLog(const char* fmt, va_list a_va)
{
	int old_size = m_Buffer.size();
	m_Buffer.appendfv(fmt, a_va);
	for (const int new_size = m_Buffer.size(); old_size < new_size; old_size++)
		if (m_Buffer[old_size] == '\n')
			m_LineOffsets.push_back(old_size + 1);
}

void LogWindow::Log(const char* fmt, ...)
{
	int old_size = m_Buffer.size();
	va_list args;
	va_start(args, fmt);
	m_Buffer.appendfv(fmt, args);
	va_end(args);
	for (const int new_size = m_Buffer.size(); old_size < new_size; old_size++)
		if (m_Buffer[old_size] == '\n')
			m_LineOffsets.push_back(old_size + 1);
}

void LogWindow::OnGUIWindow(const IMGUIInterface& a_interface)
{
	// Draw Console Scroll Rect
	const float footerReserveHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("scrolling",
		ImVec2(0, -footerReserveHeight),
		true,
		ImGuiWindowFlags_HorizontalScrollbar);
	{
		// Scrolling Child
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		{
			const char* buf = m_Buffer.begin();
			const char* buf_end = m_Buffer.end();

			if (m_Filter.IsActive()) // Filter Text
			{
				for (int line_no = 0; line_no < m_LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + m_LineOffsets[line_no];
					const char* line_end = (line_no + 1 < m_LineOffsets.Size) ? (buf + m_LineOffsets[line_no + 1] - 1) : buf_end;
					if (m_Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				ImGuiListClipper clipper;
				clipper.Begin(m_LineOffsets.Size); // Show Clipped Line Ranges only
				{
					while (clipper.Step())
					{
						for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
						{
							const char* line_start = buf + m_LineOffsets[line_no];
							const char* line_end = (line_no + 1 < m_LineOffsets.Size) ? (buf + m_LineOffsets[line_no + 1] - 1) : buf_end;
							ImGui::TextUnformatted(line_start, line_end);
						}
					}
				}
				clipper.End();
			}
		}
		ImGui::PopStyleVar();

		if (m_bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();


	ImGui::Separator();

	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &m_bAutoScroll);
		ImGui::EndPopup();
	}

	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");

	ImGui::SameLine();
	const bool bClear = ImGui::Button("Clear");
	ImGui::SameLine();
	const bool bCopy = ImGui::Button("Copy");
	ImGui::SameLine();
	m_Filter.Draw("Filter", -100.0f);

	if (bClear)
		Clear();
	if (bCopy)
		ImGui::LogToClipboard();
}
