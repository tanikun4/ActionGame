#pragma once
#include "ParticleEmitter2D.h"
#include "ParticleRenderer2D.h"

class ParticleSystem2D
{
	private :
		ParticleEmitter2D* m_emitter;
		ParticleRenderer2D* m_renderer;
		std::vector<ParticleParam2D> m_particles;
};