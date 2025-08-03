#pragma once
#include "Manager.h"

class Pole;
class Arrow;

class WeaponManager : public Manager<WeaponManager>
{
    friend class Manager<WeaponManager>;
    int EnemyCount();
    //void SetPole();
    void SetArrow();
    void Update();
    void Draw();
    void Uninit();

};

