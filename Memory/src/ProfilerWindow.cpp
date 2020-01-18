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
	ImGui::SameLine();
	DrawCycleHeapButtons(a_gui);
	
	ImGui::Separator();

	if (m_HeapData.empty()) // No Blocks, Early out
	{
		ImGui::Text("No Data");
	}
	else
	{
		ImGui::Text("Heap: %s", m_HeapData[m_CurrentHeapIdx].Name.c_str());
		const float usedSize = m_HeapData[m_CurrentHeapIdx].UsedSize / MB;
		const float capacity = m_HeapData[m_CurrentHeapIdx].Capacity / MB;
		ImGui::Text("%.2f/%.2f Mbs Used", usedSize , capacity);
		ImGui::Text("Movable: %s", m_HeapData[m_CurrentHeapIdx].Movable ? "True" : "False");
	}
	
	// Draw Frame
	ImGui::BeginChild("Heap Fragmentation", ImVec2(0, 0), true);
	
	if (m_HeapData.empty()) // No Blocks, Early out
	{
		ImGui::EndChild();
		return;
	}
	
	// Draw Heap Blocks
	DrawCurrentHeapBlocks();
	
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
			m_HeapData.clear();
			m_CurrentHeapIdx = 0;
			RecursiveBuildHeapBlocks(MemoryManager::GetDefaultHeap());
		}
	}
}

void ProfilerWindow::DrawCycleHeapButtons(const IMGUIInterface& a_gui)
{
	bool bEnableCycleLeft = false;
	bool bEnableCycleRight = false;

	if(m_HeapData.empty() == false)
	{
		bEnableCycleLeft |= m_CurrentHeapIdx > 0;
		bEnableCycleRight |= m_CurrentHeapIdx < m_HeapData.size() - 1;
	}
	
	
	if(a_gui.Button("<", bEnableCycleLeft))
	{
		m_CurrentHeapIdx--;
	}

	ImGui::SameLine();

	if(a_gui.Button(">", bEnableCycleRight))
	{
		m_CurrentHeapIdx++;
	}
}

void ProfilerWindow::DrawCurrentHeapBlocks()
{
	std::vector<Block>& Blocks = m_HeapData[m_CurrentHeapIdx].Blocks;
	
	// Get Dimensions
	const float rowW = ImGui::GetItemRectSize().x;
	const float rowHW = rowW * 0.5f;

	// Calculate Total Pixel Len
	const float TotalPixelLen = rowW * (float)m_kHeapRows;

	// Get Max Block Size
	const uint32 BlockMax = (uint32)Blocks.back().End;
	for (int i = 0; i < (int)Blocks.size(); ++i)
	{
		Block& block = Blocks[i];

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

void ProfilerWindow::RecursiveBuildHeapBlocks(const HeapBase* a_pHeap)
{
	BuildHeapBlocks(a_pHeap);
	if(a_pHeap->GetChild() != nullptr)
	{
		RecursiveBuildHeapBlocks(a_pHeap->GetChild());
	}

	if(a_pHeap->GetNextSibling() != nullptr)
	{
		RecursiveBuildHeapBlocks(a_pHeap->GetNextSibling());
	}
}

void ProfilerWindow::BuildHeapBlocks(const HeapBase* a_pHeap)
{
	HeapData& data = m_HeapData.emplace_back();
	data.Name = a_pHeap->GetName();
	data.UsedSize = a_pHeap->GetUsedMemory();
	data.Capacity = a_pHeap->GetCapacity();

	data.Blocks.reserve(a_pHeap->GetAllocationCount() * 3);
	
	const uintptr Base = (uintptr)a_pHeap->GetStartAddress();

	if(a_pHeap->GetHeadAllocation() == nullptr) // No Allocations
	{
		const Block block
		{
			(uintptr)a_pHeap->GetStartAddress() - Base,
			((uintptr)a_pHeap->GetStartAddress() + a_pHeap->GetCapacity()) - Base,
			true
		};

		data.Blocks.push_back(block);
		return;
	}
	
	if(a_pHeap->GetHeadAllocation() != a_pHeap->GetStartAddress())
	{
		// Gap Between Start Addr and Head Allocation
		Block block
		{
			block.Start = (uintptr)a_pHeap->GetStartAddress() - Base,
			block.End = (uintptr)a_pHeap->GetHeadAllocation() - Base,
			block.bFree = true
		};
		
		data.Blocks.push_back(block);
	}

	Heap::BaseAllocationHeader* pCurrent = a_pHeap->GetHeadAllocation();
	while(pCurrent != nullptr)
	{
		// Push Allocated Blocks
		Block block
		{
			block.Start = (uintptr)pCurrent - Base,
			block.End = (uintptr)pCurrent + pCurrent->Size + a_pHeap->GetAllocHeaderSize() - Base,
			block.bFree = false
		};

		data.Blocks.push_back(block);

		uintptr nextEnd;
		
		if(pCurrent->pNext != nullptr) 
		{
			nextEnd = (uintptr)pCurrent->pNext - Base;
		}
		else // Must be Tail
		{
			nextEnd = (uintptr)a_pHeap->GetStartAddress() + a_pHeap->GetCapacity() - Base; // Get End of Heap
		}

		const uint32 diff = (uint32)(nextEnd - block.End);
		if (diff != 0) // Gap Between Current and Next Allocation
		{
			block.Start = block.End;
			block.End = nextEnd;
			block.bFree = true;
			data.Blocks.push_back(block);
		}

		pCurrent = pCurrent->pNext;
	}
}

void ProfilerWindow::OnSampleRecorded()
{
	m_CurrentData = Profiler::GetCurrentFrameData();
	
	BuildSampleItemTree();
	MemoryApp::Pause();
}

void ProfilerWindow::BuildSampleItemTree()
{
	m_Root.Children.clear();
	size_t idx = 0;
	BuildSampleItemsFromDepth(m_Root, idx);
	
	m_Overhead.Data.Name = "ProfilerOverhead";
	m_Overhead.Data.Calls = 0;
	m_Overhead.Data.Depth = 0;
	m_Overhead.Data.TimeTaken = m_CurrentData.ProfilerOverhead;
	m_Overhead.TotalPercent = (m_CurrentData.ProfilerOverhead / m_CurrentData.TotalTimeTaken * 100.f);
}

bool ProfilerWindow::BuildSampleItemsFromDepth(SampleItem& a_pParent, size_t& a_idx)
{
	while(a_idx != m_CurrentData.SampleData.size())
	{
		SampleItem& item = a_pParent.Children.emplace_back();
		item.Data = m_CurrentData.SampleData[a_idx];

		item.TotalPercent = (float) (item.Data.TimeTaken / m_CurrentData.TotalTimeTaken * 100.f);

		Profiler::SampleData* pNextItemData = nullptr;

		bool bIsLastItem = a_idx == (int)m_CurrentData.SampleData.size() - 1;
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
				const bool bNextDepthLower = BuildSampleItemsFromDepth(item, a_idx);

				if(bNextDepthLower)
				{
					bIsLastItem = a_idx == (unsigned int)m_CurrentData.SampleData.size() - 1;
					if (bIsLastItem == false) // Not Last Item
					{
						pNextItemData = &m_CurrentData.SampleData[a_idx + 1u];
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
	ImGui::Separator();
	// Draw Overhead Item
	DrawSampleItem(m_Overhead);
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

