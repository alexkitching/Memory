#pragma once

#include "IMGUIWindow.h"
#include "ScenarioManager.h"

class ScenarioWindow : public IMGUIWindow
{
public:
	ScenarioWindow(ScenarioManager* a_pScenarioManager);
	~ScenarioWindow();

protected:
	void OnGUIWindow(const IMGUIInterface& a_gui) override;
private:

	void OnScenarioStarted(ScenarioType a_type);
	void OnScenarioFinished(ScenarioType a_type);

	bool m_bScenarioActive[(int)ScenarioType::COUNT];
	
	ScenarioManager* m_pScenarioManager;
};