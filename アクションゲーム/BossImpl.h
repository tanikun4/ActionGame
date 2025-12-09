#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Boss.h"

class Camera;
class Player;
class Pole;
class Boss;
class Bullet;
class TestCube;

class Boss::Impl
{ 
public:
	Impl(Camera* cam, Boss* owner);
	~Impl();

	void Init();
	void Update();
	void Draw();
	void Damage(int atk);

	void ShotBullet();
	bool GetLive();
	int GetHP();
	Pole* GetWeapon();

	void OnHit(Pole* pole);
	void OnHit(Bullet* bu);
	void OnHit(TestCube* cube);
private:
	// 所有者とカメラ参照
	Boss* m_Owner = nullptr;
	Camera* m_Camera = nullptr;
	enum STATE {
		NORMAL = 0,
		ATTACK
	};

	enum ATTACK_KIND {
		NONE = -1, //攻撃なし
		SWING = 0,
		SHOT,
		ROTATESWING,

		KIND_MAX
	};
	int hp = 10;
	int flamecount = 0;
	int m_State = 0;//状態　1で行動中
	bool inviFg = false;
	int invicount = 0;
	const float rotate_speed = 0.01;
	int attack_kind = 0;//攻撃の種類
	int attack_time = 0;//攻撃時間
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
	void SetArrow();

	void DebugBossStatus();

};

