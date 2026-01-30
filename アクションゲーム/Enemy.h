#pragma once
#include "GolfBall.h"

class Camera;

class Enemy :
    public GolfBall
{
private:
	//int hp = 10;
	//const int maxhp = 10;
	//int flamecount = 0;
	//DirectX::XMFLOAT2 stagesize = {0.0f,0.0f};
	//DirectX::SimpleMath::Vector3 hitbackrotation = { 0,0,0 };//攻撃に当たった時のノックバックする向き
	//Projectile* m_projectile = nullptr;
	//Pole* m_weapon = nullptr;

	//void Attack();
	//void SetArrow();
	//bool HitCheck();//攻撃に当たったかを返す
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
	class Pole* GetWeapon();
	bool IsAttackable();
	bool IsAttacking();
	bool IsAway();

	// Hit 系（外部から呼ばれるので public のまま）
	void HitObject(Object* ob) override { ob->OnHit(this); }

	void OnHit(Pole* po);
	void OnHit(class Bullet* bu);
	void OnHit(class Projectile* pr);
	void OnHit(class TestCube* cube);
	void OnHit(Object* ob) override {};
};

