#pragma once
#include "Collision.h"
class ICollider
{
public:
    virtual ~ICollider() {}
    virtual Collision::Base& GetCollision() = 0;
};
