#pragma once
#include "Manager.h"
#include <vector>

class Pole;
class Arrow;
class Weapon;

enum WeaponType
{
    POLE = 0,
    ARROW,
    SHIELD,

    WEAPON_MAX
};

class WeaponManager : public Manager<WeaponManager>
{
    friend class Manager<WeaponManager>;
    
public:
    void SetPole();
    void SetArrow();
    void Update();
    void Draw();
    void Uninit();
    void SetGameWeapon();
    Weapon* SetWeapon(int id);
private:
    std::vector<Weapon*> m_weapons;
};

