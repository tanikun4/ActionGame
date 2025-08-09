#include "WeaponManager.h"
#include "Arrow.h"
#include "Pole.h"
#include "Game.h"

Weapon* WeaponManager::SetWeapon(int id) {//‘½•ªg‚í‚È‚¢
	switch (id) {
	case POLE:
		m_weapons.emplace_back(new Pole());
		break;
	case ARROW:
		m_weapons.emplace_back(new Arrow());
		break;
	default:
		return nullptr;
	}
	return m_weapons.back();
}

void WeaponManager::SetGameWeapon() {
	m_weapons = Game::GetInstance()->GetObjects<Weapon>();
	return;
}