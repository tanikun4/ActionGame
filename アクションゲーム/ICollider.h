#pragma once
#include "Collision.h"
class ICollider
{
protected:

public:
    virtual ~ICollider() {}
    virtual Collision::ColliderVariant GetCollision() = 0;
};
