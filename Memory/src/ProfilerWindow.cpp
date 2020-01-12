#include "ProfilerWindow.h"
#include "Profiler.h"
#include "MemoryApp.h"

ProfilerWindow::ProfilerWindow()
	:
IMGUIWindow("Profiler"),
m_bPauseNext(false),
m_bSampleNext(false)
{
	m_Root.Data.Name = "Root";
	m_Root.Data.Depth = -1;
}

void ProfilerWindow::Initialise()
{
	Profiler::OnSampleRecorded()->AddListener<ProfilerWindow, &ProfilerWindow::OnSampleRecorded>(this);
}

void ProfilerWindow::OnGUIWindow(const IMGUIInterface& a_gui)
{
	
	if (a_gui.Button("Play", MemoryApp::IsPaused()))
	{
		MemoryApp::Play();
	}
	
	ImGui::SameLine();
	
	if(a_gui.Button("Pause", MemoryApp::IsPaused() == false))
	{
		MemoryApp::Pause();
	}

	if (a_gui.Button("Record Frame") || m_bSampleNext)
	{
		if(MemoryApp::IsPaused())
		{
			m_bSampleNext = true;
			MemoryApp::Play();
		}
		else
		{
			m_bSampleNext = false;
			MemoryApp::Pause();

			Profiler::RecordNextFrame();
		}
	}

	ImGui::Separator();

	ImGui::BeginChild("Frame Data", ImVec2(0.f, 0.f), true);
	{
		if(m_CurrentData.SampleData.size() == 0)
		{
			ImGui::Text("No Frame Data");
		}
		else
		{
			DrawCurrentSampleData();
		}
	}
	ImGui::EndChild();
	//Im
}

void ProfilerWindow::OnSampleRecorded(const Profiler::FrameData& a_data)
{
	m_CurrentData = Profiler::GetCurrentFrameData();
	
	BuildItemTree();
}

void ProfilerWindow::BuildItemTree()
{
	m_Root.Children.clear();
	int idx = 0;
	BuildItemsFromDepth(m_Root, idx);
}

void ProfilerWindow::BuildItemsFromDepth(SampleItem& a_pParent, int& a_idx)
{
	bool bContinue = true;

	while(bContinue)
	{
		bContinue = false;
		
		SampleItem& item = a_pParent.Children.emplace_back();
		item.Data = m_CurrentData.SampleData[a_idx];
		if(a_pParent.Data.Depth == m_Root.Data.Depth)
		{
			// Is Root
			item.TotalPercent = (float) (item.Data.TimeTaken / m_CurrentData.TotalTimeTaken * 100.f);
		}
		else
		{
			item.TotalPercent = (float)(item.Data.TimeTaken / m_CurrentData.TotalTimeTaken * 100.f);
		}

		Profiler::SampleData* pNextItemData = nullptr;
		if (a_idx != (int)m_CurrentData.SampleData.size() - 1) // Not Last Item
		{
			pNextItemData = &m_CurrentData.SampleData[a_idx + 1];
		}
		
		if (pNextItemData != nullptr)
		{
			a_idx++;
			if (pNextItemData->Depth == item.Data.Depth)
			{
				bContinue = true;
			}
			else if (pNextItemData->Depth > item.Data.Depth)
			{
				BuildItemsFromDepth(item, a_idx);
				bContinue = true;
			}
			else if(pNextItemData->Depth < item.Data.Depth)
			{
				return;
			}
		}

		if(a_idx == m_CurrentData.SampleData.size() - 1)
		{
			return;
		}
	}
}

void ProfilerWindow::DrawCurrentSampleData()
{
	ImGui::Columns(4);
	ImGui::Text("Sample Name");
	ImGui::NextColumn();
	ImGui::Text("Total %s", "%");
	ImGui::NextColumn();
	ImGui::Text("Time Taken ms");
	ImGui::NextColumn();
	ImGui::Text("Calls");
	ImGui::NextColumn();
	ImGui::Separator();
	for(int i = 0; i < m_Root.Children.size(); ++i)
	{
		DrawSampleItem(m_Root.Children[i]);
	}
	ImGui::Columns();
	ImGui::Separator();
	ImGui::Text("Total Frame Time: %0.2f ms", m_CurrentData.TotalTimeTaken);
}

void ProfilerWindow::DrawSampleItem(SampleItem& a_item)
{
	if(a_item.Children.size() == 0) 
	{
		ImGui::Text(a_item.Data.Name.c_str());
		ImGui::NextColumn();
		ImGui::Text("%0.2f %", a_item.TotalPercent);
		ImGui::NextColumn();
		ImGui::Text("%0.3f ms", a_item.Data.TimeTaken);
		ImGui::NextColumn();
		ImGui::Text("%i ", a_item.Data.Calls);
		ImGui::NextColumn();
	}
	else
	{
		bool bOpen = ImGui::TreeNode(a_item.Data.Name.c_str());

		ImGui::NextColumn();
		ImGui::Text("%0.2f %", a_item.TotalPercent);
		ImGui::NextColumn();
		ImGui::Text("%0.3f ms", a_item.Data.TimeTaken);
		ImGui::NextColumn();
		ImGui::Text("%i ", a_item.Data.Calls);
		ImGui::NextColumn();
		
		if(bOpen)
		{
			for(int i = 0; i < a_item.Children.size(); ++i)
			{
				DrawSampleItem(a_item.Children[i]);
			}
	
			ImGui::TreePop();
		}
	}
}

