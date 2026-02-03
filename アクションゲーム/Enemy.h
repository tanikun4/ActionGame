#pragma once
#include "BallObject.h"

class Camera;

class Enemy :
    public BallObject
{
private:
	//pimplパターン、実装を隠す
	//Implクラスの宣言
	class Impl;
	std::unique_ptr<Impl> impl;
public:
	Enemy(Camera* cam); // コンストラクタ
	~Enemy();//デストラクタ

	void Init() override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void ReInit();
	void Stun();
	void Attack();//攻撃する

	void SetTarget(Object* ta);
	int GetHP();
	class Sword* GetWeapon();
	bool IsAttackable();
	bool IsAttacking();
	bool IsAway();

	// Hit 系（外部から呼ばれるので public のまま）
	void HitObject(Object* ob) override { ob->OnHit(this); }

	void OnHit(Sword* po);
	void OnHit(class Bullet* bu);
	void OnHit(class Projectile* pr);
	void OnHit(class TestCube* cube);
	void OnHit(Object* ob) override {};
};

