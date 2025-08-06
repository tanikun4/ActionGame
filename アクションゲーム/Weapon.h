#pragma once
#include "Object.h"
#include "TestCube.h"
class Weapon :
    public Object
{
private:
    TestCube hitbox;
    int atk;
    int flamecount;
public:
};

