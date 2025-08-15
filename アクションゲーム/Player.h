#pragma once
#include "GolfBall.h"
class Arrow;
class Pole;
class Player :
    public GolfBall
{
private:
	int hp = 9;
	int flamecount = 0;
	int rollcooldown = 30;//回避のクールタイム
	int rollcount = 0;//回避関連のカウント
	bool InviFg = false;//無敵状態のフラグ
	bool GuardFg = false;//ガード状態のフラグ
	float speed = 1.0f;//自分のスピード
	void Move();
	void Attack();
	void Charge();
	void Shot();
	void CheckHit();
	void Damage(int atk);
	void DodgeRoll();//回避
	void Guard();//ガード
	Arrow* m_arrow;
	Pole* m_pole;
public:
	Player(Camera* cam); // コンストラクタ
	~Player();//デストラクタ

	void Init();
	void Update();
	int GetHP();
	Pole* GetWeapon();
	void CheckHitPole(Pole* pole);
};

