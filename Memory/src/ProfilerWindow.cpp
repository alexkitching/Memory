#include "ProfilerWindow.h"
#include "Profiler.h"
#include "MemoryApp.h"
#include "imgui_internal.h"
#include "MemoryManager.h"
#include "MemSys.h"

ProfilerWindow::ProfilerWindow()
	:
IMGUIWindow("Profiler", true, false),
m_bPauseNext(false),
m_bRecordNext(false),
m_CurrentTab(Tab::FrameProfiler)
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

	if(a_gui.Button("Frame Profiler"))
	{
		m_CurrentTab = Tab::FrameProfiler;
	}

	ImGui::SameLine();

	if (a_gui.Button("Heap Visualiser"))
	{
		m_CurrentTab = Tab::HeapVisualiser;
	}

	switch(m_CurrentTab)
	{
	case Tab::FrameProfiler:
		DrawFrameTab(a_gui);
		break;
	case Tab::HeapVisualiser:
		DrawHeapTab(a_gui);
		break;
	}
}

void ProfilerWindow::DrawFrameTab(const IMGUIInterface& a_gui)
{
	if (a_gui.Button("Record Frame") || m_bRecordNext)
	{
		if (MemoryApp::IsPaused())
		{
			m_bRecordNext = true;
			MemoryApp::Play();
		}
		else
		{
			m_bRecordNext = false;

			Profiler::RecordNextFrame();
		}
	}

	ImGui::Separator();
	
	ImGui::BeginChild("Frame Data", ImVec2(0.f, 0.f), true);
	{
		if (m_CurrentData.SampleData.empty())
		{
			ImGui::Text("No Frame Data");
		}
		else
		{
			DrawCurrentSampleData();
		}
	}

	ImGui::EndChild();
}

void ProfilerWindow::DrawHeapTab(const IMGUIInterface& a_gui)
{
	DrawRecordFragmentationButton(a_gui);

	ImGui::Separator();
	// Draw Frame
	ImGui::BeginChild("Heap Fragmentation", ImVec2(0, 0), true);
	
	if (m_Blocks.empty()) // No Blocks, Early out
	{
		ImGui::EndChild();
		return;
	}
	
	// Draw Heap Blocks
	DrawHeapBlocks();
	
	ImGui::EndChild();
}

void ProfilerWindow::DrawRecordFragmentationButton(const IMGUIInterface& a_gui)
{
	if (a_gui.Button("Record Fragmentation") || m_bRecordNext)
	{
		if(MemoryApp::IsPaused())
		{
			m_bRecordNext = true;
			MemoryApp::Play();
		}
		else
		{
			MemoryApp::Pause();
			m_bRecordNext = false;
			BuildHeapBlocks();
		}
	}
}

void ProfilerWindow::DrawHeapBlocks()
{
	// Get Dimensions
	const float rowW = ImGui::GetItemRectSize().x;
	const float rowHW = rowW * 0.5f;

	// Calculate Total Pixel Len
	const float TotalPixelLen = rowW * (float)m_kHeapRows;

	// Get Max Block Size
	const uint32 BlockMax = (uint32)m_Blocks.back().End;
	for (int i = 0; i < (int)m_Blocks.size(); ++i)
	{
		Block& block = m_Blocks[i];

		// Normalise Values within Block Range
		const float normStart = (float)block.Start / (float)BlockMax;
		const float normEnd = (float)block.End / (float)BlockMax;

		const float startPx = normStart * TotalPixelLen;
		const float endPx = normEnd * TotalPixelLen;

		if ((int)startPx - (int)endPx == 0) // Dont bother with Blocks < 1 Pixel
		{
			continue;
		}

		IMGUIQuad blockQuad = {};
		blockQuad.h = m_kRowHeight; // Height is Constant

		// Set Colour 
		blockQuad.r = 0.f;
		blockQuad.g = 0.f;
		if (block.bFree)
		{ // Blue Blocks for Free
			blockQuad.b = 255;
			blockQuad.r = 0.f;
		}
		else
		{ // Red Blocks for Allocated
			blockQuad.r = 255;
			blockQuad.b = 0.f;
		}
		blockQuad.a = 255;

		// Calulate Start/End Rows
		const int startRow = (int)(startPx / rowW);
		const int endRow = (int)(endPx / rowW);

		// Draw First Row
		const int startLeft = (int)startPx % (int)rowW;
		int endRight = (int)rowW;
		int w = endRight - startLeft;
		blockQuad.w = (float)w;
		blockQuad.x = (float)startLeft + 0.5f * (float)w;
		blockQuad.y = m_kRowStartY + (float)startRow * (m_kRowHeight + m_kRowSpacing);

		DrawQuad(blockQuad);

		if (startRow == endRow) // Only Spans a single row - continue
		{
			continue;
		}

		// Draw Completely Filled Rows
		for (int j = startRow + 1; j < endRow; ++j) 
		{
			blockQuad.w = rowW;
			blockQuad.x = 0.f + rowHW;
			blockQuad.y = m_kRowStartY + (float)j * (m_kRowHeight + m_kRowSpacing);
			DrawQuad(blockQuad);
		}
		
		// Draw End Row
		endRight = (int)endPx % (int)rowW;
		w = endRight;
		blockQuad.w = (float)w;
		blockQuad.x = 0.5f * (float)w;
		blockQuad.y = m_kRowStartY + endRow * (m_kRowHeight + m_kRowSpacing);
		DrawQuad(blockQuad);
	}
}

void ProfilerWindow::BuildHeapBlocks()
{
	Heap* pDefaultHeap = MemoryManager::GetDefaultHeap();
	m_Blocks.clear();
	m_Blocks.reserve(pDefaultHeap->GetAllocationCount() * 3);
	
	const uintptr Base = (uintptr)pDefaultHeap->GetStartAddress();

	if(pDefaultHeap->GetHeadAllocation() != pDefaultHeap->GetStartAddress())
	{
		// Gap Between Start Addr and Head Allocation
		Block block
		{
			block.Start = (uintptr)pDefaultHeap->GetStartAddress() - Base,
			block.End = (uintptr)pDefaultHeap->GetHeadAllocation() - Base,
			block.bFree = true
		};
		
		m_Blocks.push_back(block);
	}

	Heap::AllocationHeader* pCurrent = pDefaultHeap->GetHeadAllocation();
	while(pCurrent != nullptr)
	{
		// Push Allocated Blocks
		Block block
		{
			block.Start = (uintptr)pCurrent - Base,
			block.End = (uintptr)pCurrent + pCurrent->Size + HEAP_ALLOC_HEADER_FOOTER_SIZE - Base,
			block.bFree = false
		};

		m_Blocks.push_back(block);

		if(pCurrent->pNext != nullptr) 
		{
			const uint32 diff = (uint32)((uintptr)pCurrent->pNext - Base - block.End);
			if(diff != 0) // Gap Between Current and Next Allocation
			{
				block.Start = block.End;
				block.End = (uintptr)pCurrent->pNext - Base;
				block.bFree = true;
				m_Blocks.push_back(block);
			}
		}

		pCurrent = pCurrent->pNext;
	}

	pCurrent = pDefaultHeap->GetTailAllocation();
	const uintptr TailEnd = (uintptr)pCurrent + pCurrent->Size + HEAP_ALLOC_HEADER_FOOTER_SIZE;
	const uintptr End = (uintptr)pDefaultHeap->GetStartAddress() + pDefaultHeap->GetCapacity();
	if(TailEnd != End)
	{
		Block block
		{
			block.Start = TailEnd - Base,
		block.End = End - Base,
		block.bFree = true
		};
		
		m_Blocks.push_back(block);
	}
}

void ProfilerWindow::OnSampleRecorded()
{
	m_CurrentData = Profiler::GetCurrentFrameData();
	
	BuildItemTree();
	MemoryApp::Pause();
}

void ProfilerWindow::BuildItemTree()
{
	m_Root.Children.clear();
	int idx = 0;
	BuildItemsFromDepth(m_Root, idx);
}

bool ProfilerWindow::BuildItemsFromDepth(SampleItem& a_pParent, int& a_idx)
{
	while(a_idx != m_CurrentData.SampleData.size())
	{
		SampleItem& item = a_pParent.Children.emplace_back();
		item.Data = m_CurrentData.SampleData[a_idx];

		item.TotalPercent = (float) ((item.Data.TimeTaken / item.Data.Calls)/ m_CurrentData.TotalTimeTaken * 100.f);

		Profiler::SampleData* pNextItemData = nullptr;

		const bool bIsLastItem = a_idx == (int)m_CurrentData.SampleData.size() - 1;
		if (bIsLastItem == false) // Not Last Item
		{
			pNextItemData = &m_CurrentData.SampleData[a_idx + 1];
		}
		else
		{
			a_idx++;
			break;
		}
		
		if (pNextItemData != nullptr)
		{
			if (pNextItemData->Depth < item.Data.Depth)
			{
				return true;
			}

			a_idx++;
			
			if (pNextItemData->Depth > item.Data.Depth)
			{
				const bool bNextDepthLower = BuildItemsFromDepth(item, a_idx);

				if(bNextDepthLower)
				{
					const bool bIsLastItem = a_idx == (int)m_CurrentData.SampleData.size() - 1;
					if (bIsLastItem == false) // Not Last Item
					{
						pNextItemData = &m_CurrentData.SampleData[a_idx + 1];
					}

					if (pNextItemData->Depth < item.Data.Depth)
					{
						return true;
					}
					
					a_idx++; // Now Not Lower
				}
			}
		}
	}
	return false;
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
	for(int i = 0; i < (int)m_Root.Children.size(); ++i)
	{
		DrawSampleItem(m_Root.Children[i]);
	}
	ImGui::Columns();
	ImGui::Separator();
	ImGui::Text("Total Frame Time: %0.3f ms", m_CurrentData.TotalTimeTaken);
	ImGui::Text("Profiler Overhead: %0.3f ms", m_CurrentData.ProfilerOverhead);
}

void ProfilerWindow::DrawSampleItem(SampleItem& a_item)
{
	if(a_item.Children.empty()) 
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
			for(int i = 0; i < (int)a_item.Children.size(); ++i)
			{
				DrawSampleItem(a_item.Children[i]);
			}
	
			ImGui::TreePop();
		}
	}
}

