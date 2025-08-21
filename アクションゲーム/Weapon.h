#pragma once
#include "Object.h"
#include "TestCube.h"
#include "ICollider.h"
#include "Collision.h"

class Weapon :
    public Object , public ICollider
{
private:
    TestCube hitbox;
    int atk;
    int flamecount;
public:
    Weapon(Camera* cam);//コンストラクタ
    Weapon();
    ~Weapon();//デストラクタ

    Collision::Base& GetCollision() override ;
};

