#pragma once
#include <vector>

class ParticleParam2D;
class ParticleRenderer2D
{
public:
    void Draw(const std::vector<ParticleParam2D>& particles);
};
