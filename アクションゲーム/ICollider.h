#pragma once
#include "Collision.h"

class ICollider
{

public:
    virtual ~ICollider() {}
    virtual Collision::ColliderVariant GetCollision() const = 0;

    // Õ“ËŒ‹‰Ê‚Ì•Û‘¶‚Ææ“¾
    void SetCollisionResult(const CollisionResult& result) { m_LastCollision = result; }
    const CollisionResult& GetLastCollision() const { return m_LastCollision; }

private:
    CollisionResult m_LastCollision;
};
