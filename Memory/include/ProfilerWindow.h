#pragma once

#include "IMGUIWindow.h"
#include "Profiler.h"
#include "Types.h"
#include <vector>

class HeapBase;

class ProfilerWindow : public IMGUIWindow
{
public:
	ProfilerWindow();

	void Initialise();

	void OnGUIWindow(const IMGUIInterface& a_gui) override;

private:
	enum class Tab
	{
		FrameProfiler,
		HeapVisualiser
	};

	struct SampleItem
	{
		SampleItem()
		{
			TotalPercent = 0.f;
		}
		
		Profiler::SampleData Data;
		float TotalPercent;
		std::vector<SampleItem> Children;
	};

	void DrawFrameTab(const IMGUIInterface& a_gui);

	// Heap Visualisation
	void DrawHeapTab(const IMGUIInterface& a_gui);
	void DrawRecordFragmentationButton(const IMGUIInterface& a_gui);
	void DrawCycleHeapButtons(const IMGUIInterface& a_gui);
	void DrawCurrentHeapBlocks();

	void RecursiveBuildHeapBlocks(const HeapBase* a_pHeap);
	void BuildHeapBlocks(const HeapBase* a_pHeap);

	void OnSampleRecorded();

	void BuildSampleItemTree();
	bool BuildSampleItemsFromDepth(SampleItem& a_pParent, size_t& a_idx);

	void DrawCurrentSampleData();
	void DrawSampleItem(SampleItem& a_item);
	
	bool m_bPauseNext;
	bool m_bRecordNext;

	Profiler::FrameData m_CurrentData;
	SampleItem m_Root;
	SampleItem m_Overhead;

	Tab m_CurrentTab;

	struct Block
	{
		uintptr Start;
		uintptr End;
		bool bFree;
	};

	struct HeapData
	{
		std::string Name;
		bool Movable;
		size_t UsedSize;
		size_t Capacity;
		std::vector<Block> Blocks;
	};

	std::vector<HeapData> m_HeapData;
	unsigned int m_CurrentHeapIdx;

	const int m_kHeapRows = 45;
	// Formatting
	const float m_kRowStartY = 10.f;
	const float m_kRowHeight = 10.f;
	const float m_kRowSpacing = 2.f;
};
