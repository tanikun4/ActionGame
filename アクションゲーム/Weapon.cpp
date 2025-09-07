#include "Weapon.h"

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


Collision::ColliderVariant Weapon::GetCollision() {
	return Collision::OBB { m_Position, m_Rotation,m_Scale };
}