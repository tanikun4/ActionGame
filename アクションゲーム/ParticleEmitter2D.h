#pragma once
#include <vector>
#include "MathCommon.h"
#include "ParticleparamStruct.h"

class ParticleEmitter2D
{
public:
    void Emit(std::vector<ParticleParam2D>& particles);
    
    void Init(const ParticleEmitterParam2D& param);
    void Update(float dt);

    void Emit();
    void Play();   // î≠ê∂äJén
    void Stop();   // âië±í‚é~

    std::vector<ParticleParam2D>& GetParticles();

private:
    ParticleEmitterParam2D m_param;

    std::vector<ParticleParam2D> m_particles;

    float m_timer = 0.0f;
    bool  m_active = false;
};
