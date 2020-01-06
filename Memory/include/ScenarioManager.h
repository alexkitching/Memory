#pragma once
#include "ResourceLoadingScenario.h"
#include "Common.h"

class ScenarioManager
{
public:

	enum class ScenarioType
	{
		Invalid = -1,
		ResourceLoadingBootup,
		ResourceLoadingGameplay,
		ParticleSystem,
		VertexDataProcessing,
		COUNT
	};




	ScenarioManager();
	~ScenarioManager();

	void StartScenario(ScenarioType a_type);
	void StopScenario(ScenarioType a_type);

	void Update();
	
private:

	struct ActiveScenario
	{
		ActiveScenario(IScenario* a_pScenario, ScenarioType a_type)
			:
		pScenario(a_pScenario),
		Type( a_type)
		{
			Timer.Start();
		}
		IScenario* pScenario;
		ScenarioType Type;
		Timer Timer;
	};

	std::string ScenarioTypeNames[(int)ScenarioType::COUNT] =
	{
		{TEXT(ResourceLoadingBootup)},
		{TEXT(ResourceLoadingGameplay)},
		{TEXT(ParticleSystem)},
		{TEXT(VertexDataProcessing)}
	};

	IScenario* GetScenario(ScenarioType a_type);
	
	struct
	{
		ResourceLoadingScenario ResourceLoading;
	}Scenarios;

	std::vector<ActiveScenario> m_ActiveScenarios;
};
