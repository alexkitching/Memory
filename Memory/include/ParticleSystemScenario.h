#pragma once
#include "Scenario.h"
#include "Timer.h"
#include <vector>
#include "GlobalTime.h"
#include "RandomUtility.h"
#include "Debug.h"

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

	void Run() override
	{
		if(m_RunTimeTimer.IsStarted() == false) // Begin
		{
			Initialise();
			m_RunTimeTimer.Start();
		}

		m_RunTimeTimer.Tick();

		// Update Particle Systems
		int iTotalParticles = 0;
		for(auto& ps : m_ParticleSystems)
		{
			ps.Update();
			iTotalParticles += ps.GetParticleCount();
		}

		LOG("Total Particle Count: %i \n", iTotalParticles);
		
		if (m_RunTimeTimer.GetTime() >= m_Config.RunLength) 
		{
			m_bComplete = true;
		}
	}
	
	void Reset() override
	{
		m_bComplete = false;
		m_RunTimeTimer.Stop();
		
		m_ParticleSystems.clear();
	}
	
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

		void Update()
		{
			m_NextParticleTimer.Tick();
			
			// Spawn Particles
			if((int)m_Particles.size() < m_Config.MaxParticles) // Not Reached Max Particles
			{
				if(m_NextParticleTimer.IsStarted() == false) // Ensure Timer is Started
				{
					m_NextParticleTimer.Start();
				}

				float fNextTime = m_NextParticleTimer.GetTime();
				const float fInterval = 1.f / m_Config.ParticleEmissionRatePerSecond;
				if (fNextTime >= fInterval) // Timer Elapsed
				{
					float fIntervals = fNextTime / fInterval;
					int SpawnCount = (int)fIntervals;
					for(int i = 0; i < SpawnCount; ++i)
					{
						SpawnParticle();
					}
					

					m_NextParticleTimer.SetTime(fIntervals - (float)SpawnCount);
				}
			}

			for(auto& p : m_Particles)
			{
				p.LiveTime += Time::DeltaTime();
				p.Position[0] += p.Velocity[0] += Time::DeltaTime();
				p.Position[1] += p.Velocity[1] += Time::DeltaTime();
				p.Position[2] += p.Velocity[2] += Time::DeltaTime();
			}

			for(int i = (int)m_Particles.size() - 1; i >= 0; --i)
			{
				Particle* p = &m_Particles[i];
				if(p->LiveTime >= m_Config.LifeTime)
				{
					DestroyParticle(i);
				}
			}
			
		}

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

		void SpawnParticle()
		{
			// Push new Particle
			m_Particles.emplace_back(m_Config.OriginPosition[0], m_Config.OriginPosition[1], m_Config.OriginPosition[2]);

			// Stop Timer
			m_NextParticleTimer.Stop();
			if ((int)m_Particles.size() < m_Config.MaxParticles) // Still Not Reached Max - Start Timer
			{
				m_NextParticleTimer.Start();
			}
		}

		void DestroyParticle(int a_pIdx)
		{
			m_Particles.erase(m_Particles.begin() + a_pIdx);
		}
		
		Config m_Config;
		
		DT_Timer m_NextParticleTimer;

		std::vector<Particle> m_Particles;
	};

	void Initialise()
	{
		while ((int)m_ParticleSystems.size() != m_Config.ParticleSystemsCount)
		{
			ParticleSystem::Config config =
			{
				m_Config.ParticleSystem.MaxParticles,
				m_Config.ParticleSystem.ParticleStartCount,
				m_Config.ParticleSystem.ParticleEmissionRate,
				 m_Config.ParticleSystem.ParticleLifeTime,
			};
			m_ParticleSystems.emplace_back(config);
		}
	}
	
	Config m_Config;
	DT_Timer m_RunTimeTimer;
	bool m_bComplete;

	std::vector<ParticleSystem> m_ParticleSystems;
};
