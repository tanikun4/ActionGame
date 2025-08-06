#pragma once
class ICollider
{
public:
    virtual ~ICollider() {}
    virtual void HitCheck(int damage) = 0;
};
