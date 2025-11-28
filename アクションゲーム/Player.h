#pragma once
#include <memory>
#include "GolfBall.h"

class Camera;
class Pole;
class Boss;
class Bullet;
class TestCube;
class PlayerImpl;

class Player : public GolfBall
{
public:
    Player(Camera* cam);
    ~Player();

    void Init() override;
    void Update() override;
    void Uninit() override;

    int GetHP();
    Pole* GetWeapon();

    // Hit ŒniŠO•”‚©‚çŒÄ‚Î‚ê‚é‚Ì‚Å public ‚Ì‚Ü‚Üj
    void HitObject(Object* ob) override { ob->OnHit(this); }

    void OnHit(Boss* bo);
    void OnHit(Pole* po);
    void OnHit(Bullet* bu);
    void OnHit(TestCube* cube);
    void OnHit(Object* ob) override {};

private:
    class Impl;
    std::unique_ptr<Impl> impl;

};
