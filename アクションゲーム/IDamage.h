#pragma once
class IDamage
{
public:
    virtual ~IDamage() {}
    virtual void Damage(int damage) = 0;
    virtual bool GetLive() const = 0;
};

