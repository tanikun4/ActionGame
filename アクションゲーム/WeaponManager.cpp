#include "WeaponManager.h"
#include "Projectile.h"
#include "Sword.h"
#include "Game.h"

Weapon* WeaponManager::SetWeapon(int id) {//‘½•ªg‚í‚È‚¢
	switch (id) {
	case POLE:
		m_weapons.emplace_back(new Sword());
		break;
	case PROJECTILE:
		m_weapons.emplace_back(new Projectile());
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