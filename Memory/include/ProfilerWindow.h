#pragma once

#include "IMGUIWindow.h"
#include "Profiler.h"
#include <vector>

class ProfilerWindow : public IMGUIWindow
{
public:
	ProfilerWindow();

	void Initialise();

	void OnGUIWindow(const IMGUIInterface& a_gui) override;

private:

	struct SampleItem
	{
		Profiler::SampleData Data;
		float TotalPercent;
		std::vector<SampleItem> Children;
	};

	void OnSampleRecorded(const Profiler::FrameData& a_data);

	void BuildItemTree();
	void BuildItemsFromDepth(SampleItem& a_pParent, int& a_idx);

	void DrawCurrentSampleData();
	void DrawSampleItem(SampleItem& a_item);
	
	bool m_bPauseNext;
	bool m_bSampleNext;

	Profiler::FrameData m_CurrentData;
	SampleItem m_Root;
};