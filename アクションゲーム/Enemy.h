#pragma once
#include "GolfBall.h"
class Projectile;
class Pole;

class Enemy :
    public GolfBall
{
private:
	int hp = 10;
	const int maxhp = 10;
	int flamecount = 0;
	DirectX::XMFLOAT2 stagesize = {0.0f,0.0f};
	DirectX::SimpleMath::Vector3 hitbackrotation = { 0,0,0 };//攻撃に当たった時のノックバックする向き
	Projectile* m_projectile = nullptr;
	Pole* m_weapon = nullptr;

	void Attack();
	void SetArrow();
	bool HitCheck();//攻撃に当たったかを返す
public:
	Enemy(Camera* cam); // コンストラクタ
	~Enemy();//デストラクタ

	void Init();
	void Update();
	void Draw();
	void Damage(int atk);
	void SetStageSize(DirectX::XMFLOAT2 _stagesize);
	bool GetLive();//生存しているかを返す
};

