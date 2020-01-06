#pragma once
#include "ResourceLoadingScenario.h"

class ScenarioManager
{
public:

	
	enum class ScenarioType
	{
		Invalid = -1,
		ResourceLoadingBootup,
		ResourceLoadingGameplay,
		ParticleSystem,
		VertexDataProcessing
	};
	
	ScenarioManager();
	~ScenarioManager();

	void StartScenario(ScenarioType a_type);
	void StopScenario(ScenarioType a_type);

	void Update();
	
private:

	IScenario* GetScenario(ScenarioType a_type);
	
	struct
	{
		ResourceLoadingScenario ResourceLoading;
	}Scenarios;

	std::vector<IScenario*> m_ActiveScenarios;
};
