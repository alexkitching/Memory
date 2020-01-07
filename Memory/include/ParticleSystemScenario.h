#pragma once
#include "Scenario.h"
#include "Timer.h"
#include "RandomUtility.h"
#include <vector>

class ParticleSystemScenario : public IScenario
{
public:
	struct Config
	{
		int ParticleSystemsCount;
		struct
		{
			int MaxParticles;
			float ParticleEmissionRate;
			int ParticleStartCount;
			float ParticleLifeTime;
		}ParticleSystem;

		float RunLength;
	};

	ParticleSystemScenario() = default;
	ParticleSystemScenario(const Config& a_config)
	:
	m_Config(a_config),
	m_bComplete(false)
	{}
	
	virtual ~ParticleSystemScenario() {}

	void Run() override;
	void OnRender(IRenderer* a_pRenderer) override;
	void Reset() override;
	bool IsComplete() override { return m_bComplete; }

private:
	class ParticleSystem
	{
	public:
		struct Config
		{
			int MaxParticles;
			int StartParticles;
			float ParticleEmissionRatePerSecond;
			float LifeTime;
			float OriginPosition[3];
		};
		
		ParticleSystem(const Config& a_config)
			:
		m_Config(a_config)
		{
			while((int)m_Particles.size() != m_Config.StartParticles)
			{
				SpawnParticle();
			}
		}
		~ParticleSystem() {}

		void Update();
		void OnRender(IRenderer* a_pRenderer);

		int GetParticleCount() const { return (int)m_Particles.size(); }
		
	private:
		struct Particle
		{
			Particle(float a_fX, float a_fY, float a_fZ)
				:
			Position{a_fX, a_fY, a_fZ},
			Velocity{Random::FloatRange(-1.f, 1.f), // Random Dirs
					 Random::FloatRange(-1.f, 1.f) ,
					 Random::FloatRange(-1.f, 1.f) },
			LiveTime(0.f)
			{
				// Normalise Dir
				const float len = Velocity[0] + Velocity[1] + Velocity[2];
				Velocity[0] /= len;
				Velocity[1] /= len;
				Velocity[2] /= len;
			}
			float Position[3];
			float Velocity[3];
			float LiveTime;
		};

		void SpawnParticle();
		void DestroyParticle(int a_pIdx);

		Config m_Config;
		DT_Timer m_NextParticleTimer;
		std::vector<Particle> m_Particles;
	};

	void Initialise();
	
	Config m_Config;
	DT_Timer m_RunTimeTimer;
	bool m_bComplete;

	std::vector<ParticleSystem> m_ParticleSystems;
};
