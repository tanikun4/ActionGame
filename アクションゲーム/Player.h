#pragma once
#include <memory>
#include "BallObject.h"

class Camera;
class PlayerImpl;

class Player : public BallObject
{
public:
    Player(Camera* cam);
    ~Player();

    void Init() override;
    void Update() override;
	void Draw() override;
    void Uninit() override;

	void SetGauge();//ゲージ初期化用、ゲームシーンでのみ呼び出す
    void SetDemoMode(const bool _demoMode);
	std::vector<Texture2D*> GetGauge();

    int GetHP();
    Sword* GetWeapon();

    // Hit 系（外部から呼ばれるので public のまま）
    void HitObject(Object* ob) override { ob->OnHit(this); }

    void OnHit(class Enemy* en);
    void OnHit(class Boss* bo);
    void OnHit(class Sword* po);
    void OnHit(class Bullet* bu);
	void OnHit(class Projectile* pr);
    void OnHit(class Cube* cube);
    void OnHit(Object* ob) override {};

private:
    //pimplパターン、実装を隠す
    //Implクラスの宣言
    class Impl;
    std::unique_ptr<Impl> impl;

};
