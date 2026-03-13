#include "Weapon.h"
#include "BallObject.h"

//コンストラクタ
Weapon::Weapon(Camera* cam) : Object(cam) {

}

Weapon::Weapon()
{

}

// デストラクタ
Weapon::~Weapon()
{

}

// 色の設定
void Weapon::SetColor(DirectX::SimpleMath::Vector4 color, int index) {
	if(index >= 0 && index < (int)m_Materiales.size()) {
		m_Materiales[index]->SetDiffuse(color);
		m_Materiales[index]->Update();
		return;
	}
	else {
		for (auto& m : m_Materiales) {
			m->SetDiffuse(color);
			m->Update();
		}
	}
	return;
}

void Weapon::SetOwner(BallObject* owner) { m_Owner = owner; }// 所有者をセット

BallObject* Weapon::GetOwner() { return m_Owner; }

Collision::ColliderVariant Weapon::GetCollision() const{
	return Collision::OBB { m_Position, m_Rotation,m_Scale };
}