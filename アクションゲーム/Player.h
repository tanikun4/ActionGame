#pragma once
#include "GolfBall.h"
class Arrow;
class Pole;
class Boss;
class Bullet;

class Player :
    public GolfBall
{
private:
	int hp = 9;
	int flamecount = 0;
	int invicount = 0;//無敵時間のカウント
	int rollcooldown = 30;//回避のクールタイム
	int rollcount = 0;//回避関連のカウント
	int guardcount = 0;//ガード時間のカウント
	bool inviFg = false;//無敵状態のフラグ
	bool GuardFg = false;//ガード状態のフラグ
	bool RollFg = false;//回避状態のフラグ
	float speed = 1.0f;//自分のスピード
	void Move();
	void Attack();
	void Charge();
	void Shot();
	void CheckHit();
	void Damage(int atk);
	void DodgeRoll();//回避
	void Guard();//ガード
	void Counter();//カウンター攻撃
	void LookAt(DirectX::SimpleMath::Vector3 ta_pos);//ターゲットの方向を見る

	DirectX::SimpleMath::Vector3 m_ta_pos;//接近の目標点
	Bullet* m_arrow;
	Pole* m_pole;

public:
	Player(Camera* cam); // コンストラクタ
	~Player();//デストラクタ

	void Init();
	void Update();
	int GetHP();
	Pole* GetWeapon();
	void CheckHitPole(Pole* pole);
	void HitObject(Object* ob) override {
		ob->OnHit(this);
	}
	void OnHit(Object* ob) override {};
	void OnHit(Boss* boss);
	void OnHit(Pole* pole);
	void OnHit(Bullet* bu);
};

