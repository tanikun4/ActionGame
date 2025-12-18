#include "ParticleEmitter2D.h"
#include "ParticleparamStruct.h"

using namespace DirectX::SimpleMath;

void ParticleEmitter2D::Emit(std::vector<ParticleParam2D>& particles) 
{
    for (int i = 0; i < m_EmitCount; ++i)
    {
        ParticleParam2D p{};
        p.pos = m_Position;

        p.velocity = {
			(rand() % 2 + 1.0f),
			(rand() % 2 + 1.0f),
        };

        p.rot = 0.0f;
        p.scale = Vector2(10.0f,10.0f);
        p.maxLife = 60.0f;
        p.life = p.maxLife;
        p.color = 0xffffffff;

        particles.emplace_back(p);
    }
}