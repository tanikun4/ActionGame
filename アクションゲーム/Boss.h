#pragma once
#include "Golfball.h"
#include <vector>
class Arrow;
class Pole;
class Bullet;

class Boss :
    public GolfBall
{
private:
	enum STATE {
		IDLE = 0,
		ATTACK
	};

	enum ATTACK_KIND {
		SWING = 0,
		SHOT,
		KIND_MAX
	};
	int hp = 10;
	int flamecount = 0;
	int m_State = 0;//状態　1で行動中
	bool inviFg = false;
	int invicount = 0;
	const float rotate_speed = 0.01;
	int attack_kind = 0;//攻撃の種類

	bool notUpdate = false;//更新を止めるかどうか
	DirectX::SimpleMath::Vector3 m_destrot;//回転の目標点
	DirectX::XMFLOAT2 stagesize;
	DirectX::SimpleMath::Vector3 hitbackrotation = { 0,0,0 };//攻撃に当たった時のノックバックする向き
	std::vector<Bullet*> m_bullet;
	Pole* m_weapon;

	const float gravity = -0.007f;

	void LookAt(DirectX::SimpleMath::Vector3);
	void Move();
	void AttackUpdate();
	void Stance();//構え状態になる
	void SetArrow();
	bool HitCheck();//攻撃に当たったかを返す

	void DebugBossStatus();
public:
	Boss(Camera * cam); // コンストラクタ
	~Boss();//デストラクタ

	void Init();
	void Update();
	void Draw();
	void Damage(int atk);

	void ShotBullet();
	bool GetLive();
	int GetHP();
	bool HitCheckPole(Pole* pole);//攻撃に当たったかを返す
	Pole* GetWeapon();
	void HitObject(Object* ob) override {
		ob->OnHit(this);
	}
	void OnHit(Pole* pole);
	void OnHit(Bullet* bu);
	void OnHit(TestCube* cube);
};

