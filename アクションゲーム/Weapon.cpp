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


Collision::Base& Weapon::GetCollision() {
	Collision::OBB obb = { m_Position, m_Rotation,m_Scale };
	return obb;
}