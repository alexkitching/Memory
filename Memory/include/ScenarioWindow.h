#pragma once

#include "IMGUIWindow.h"
#include "ScenarioManager.h"

//------------
// Description
//--------------
// Scenario Window, provides functionality of starting/stopping scenarios and configuring them.
//------------

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

	// Draw Each Scenarios Buttons
	void DrawBootupResourceLoadingScenario(const IMGUIInterface& a_gui);
	void DrawGameplayResourceLoadingScenario(const IMGUIInterface& a_gui);
	void DrawParticleSystemScenario(const IMGUIInterface& a_gui);
	void DrawVertexDataProcessingScenario(const IMGUIInterface& a_gui);
	
	inline bool IsolatedScenarioRunning(ScenarioType a_checkingScenario) const;
	inline bool AnyOtherScenarioRunning(ScenarioType a_checkingScenario)const;
	
	bool m_bScenarioActive[(int)ScenarioType::COUNT];
	ScenarioType m_ActiveIsolatedScenario;
	
	ScenarioManager* m_pScenarioManager;
};