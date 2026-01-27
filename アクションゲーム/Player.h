#pragma once
#include <memory>
#include "GolfBall.h"

class Camera;
class Pole;
class Boss;
class Bullet;
class TestCube;
class PlayerImpl;
class Projectile;

class Player : public GolfBall
{
public:
    Player(Camera* cam);
    ~Player();

    void Init() override;
    void Update() override;
	void Draw() override;
    void Uninit() override;

	void SetGauge();//ゲージ初期化用、ゲームシーンでのみ呼び出す
    void SetDemoMode(bool _demoMode);
	std::vector<Texture2D*> GetGauge();

    int GetHP();
    Pole* GetWeapon();

    // Hit 系（外部から呼ばれるので public のまま）
    void HitObject(Object* ob) override { ob->OnHit(this); }

    void OnHit(Boss* bo);
    void OnHit(Pole* po);
    void OnHit(Bullet* bu);
	void OnHit(Projectile* pr);
    void OnHit(TestCube* cube);
    void OnHit(Object* ob) override {};

private:
    //pimplパターン、実装を隠す
    //Implクラスの宣言
    class Impl;
    std::unique_ptr<Impl> impl;

};
