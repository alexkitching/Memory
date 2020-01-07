#include "ParticleSystemScenario.h"
#include "Debug.h"
#include "GlobalTime.h"
#include "Renderer.h"

void ParticleSystemScenario::Run()
{
	if (m_RunTimeTimer.IsStarted() == false) // Begin
	{
		Initialise();
		m_RunTimeTimer.Start();
	}

	m_RunTimeTimer.Tick();

	// Update Particle Systems
	int iTotalParticles = 0;
	for (auto& ps : m_ParticleSystems)
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

void ParticleSystemScenario::OnRender(IRenderer* a_pRenderer)
{
	for(auto& ps : m_ParticleSystems)
	{
		ps.OnRender(a_pRenderer);
	}
}

void ParticleSystemScenario::Reset()
{
	m_bComplete = false;
	m_RunTimeTimer.Stop();
	m_ParticleSystems.clear();
}

void ParticleSystemScenario::ParticleSystem::Update()
{
	m_NextParticleTimer.Tick();

	// Spawn Particles
	if ((int)m_Particles.size() < m_Config.MaxParticles) // Not Reached Max Particles
	{
		if (m_NextParticleTimer.IsStarted() == false) // Ensure Timer is Started
		{
			m_NextParticleTimer.Start();
		}

		const float fNextTime = m_NextParticleTimer.GetTime();
		const float fInterval = 1.f / m_Config.ParticleEmissionRatePerSecond;
		if (fNextTime >= fInterval) // Timer Elapsed
		{
			const float fIntervals = fNextTime / fInterval;
			const int SpawnCount = (int)fIntervals;
			for (int i = 0; i < SpawnCount; ++i)
			{
				SpawnParticle();
			}


			m_NextParticleTimer.SetTime(fIntervals - (float)SpawnCount);
		}
	}

	for (auto& p : m_Particles)
	{
		p.LiveTime += Time::DeltaTime();
		p.Position[0] += p.Velocity[0] += Time::DeltaTime();
		p.Position[1] += p.Velocity[1] += Time::DeltaTime();
		p.Position[2] += p.Velocity[2] += Time::DeltaTime();
	}

	for (int i = (int)m_Particles.size() - 1; i >= 0; --i)
	{
		Particle* p = &m_Particles[i];
		if (p->LiveTime >= m_Config.LifeTime)
		{
			DestroyParticle(i);
		}
	}

}

void ParticleSystemScenario::ParticleSystem::OnRender(IRenderer* a_pRenderer)
{
	for(auto& p : m_Particles)
	{
		// TODO Optimise Renderer KILLS FPS probs due to Constant/create/destroy of buffers
		//a_pRenderer->DrawCube(p.Position[0], p.Position[1], p.Position[2], 0.1f, Time::DeltaTime());
	}
}

void ParticleSystemScenario::ParticleSystem::SpawnParticle()
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

void ParticleSystemScenario::ParticleSystem::DestroyParticle(int a_pIdx)
{
	m_Particles.erase(m_Particles.begin() + a_pIdx);
}

void ParticleSystemScenario::Initialise()
{
	while ((int)m_ParticleSystems.size() != m_Config.ParticleSystemsCount)
	{
		ParticleSystem::Config config =
		{
			m_Config.ParticleSystem.MaxParticles,
			m_Config.ParticleSystem.ParticleStartCount,
			m_Config.ParticleSystem.ParticleEmissionRate,
			m_Config.ParticleSystem.ParticleLifeTime,
		{0.f} // Zero Vector
		};
		m_ParticleSystems.emplace_back(config);
	}
}

