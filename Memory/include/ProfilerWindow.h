#pragma once

#include "IMGUIWindow.h"
#include "Profiler.h"
#include "Types.h"
#include <vector>

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
		Profiler::SampleData Data;
		float TotalPercent;
		std::vector<SampleItem> Children;
	};

	void DrawFrameTab(const IMGUIInterface& a_gui);

	// Heap Visualisation
	void DrawHeapTab(const IMGUIInterface& a_gui);
	void DrawRecordFragmentationButton(const IMGUIInterface& a_gui);
	void DrawHeapBlocks();
	void BuildHeapBlocks();

	void OnSampleRecorded();

	void BuildItemTree();
	bool BuildItemsFromDepth(SampleItem& a_pParent, int& a_idx);

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
	std::vector<Block> m_Blocks;
	const int m_kHeapRows = 45;
	// Formatting
	const float m_kRowStartY = 10.f;
	const float m_kRowHeight = 10.f;
	const float m_kRowSpacing = 2.f;
};
