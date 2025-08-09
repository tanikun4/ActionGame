#pragma once

template <typename T>
class ICollider
{
public:
    virtual ~ICollider() {}
    virtual T GetCollision() = 0;
};
