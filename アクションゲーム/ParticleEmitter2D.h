#pragma once
#include <vector>

class ParticleParam2D;
class ParticleEmitter2D
{
public:
    void Emit(std::vector<ParticleParam2D>& particles);
};
