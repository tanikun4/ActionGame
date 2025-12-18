#pragma once
#include <vector>

class ParticleParam2D;
class ParticleEmitter2D
{
public:
    void Emit(std::vector<ParticleParam2D>& particles);

private:
    DirectX::SimpleMath::Vector3 m_Position;
	int m_EmitCount = 10;
};
