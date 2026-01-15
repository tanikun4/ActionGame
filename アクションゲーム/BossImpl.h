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
	void Damage(int _atk);

	void SetDEF(int _def) { def = _def; }

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
		NORMAL = 0, //通常状態
		ATTACK,		// 攻撃中
		STUN,		// 行動不能、ジャストガードされると移行する。
	};

	enum ATTACK_KIND {
		NONE = -1, //攻撃なし
		SWING = 0,
		ROTATESWING,
		SWING_VERTICAL,
		MANY_THRUST,
		THREE_SWING,

		KIND_MAX
	};
	int hp = 50;
	int def = 0; //防御力、値分ダメージを減らす
	int m_stateframe = 0;//状態継続フレーム数
	int m_state = 0;//状態　1で行動中
	bool inviFg = false;
	int invicount = 0;
	float move_speed = 0.25f;
	float rotate_speed = 0.01;
	int attack_kind = 0;//攻撃の種類
	int attack_time = 0;//攻撃時間
	int attack_count = 0;//攻撃回数のカウント
	bool notUpdate = false;//更新を止めるかどうか
	bool m_lookatFg = true;//プレイヤーへの追従をするかどうか
	bool m_rushFg = false;//突進しているかどうか


	bool m_slowFg = false;//動きが遅い状態か
	int slow_frame = 0;//遅くなっているフレーム数
	int slow_rate = 6;//元の速度の何分の1にするか

	DirectX::SimpleMath::Vector3 m_destrot;//回転の目標点
	DirectX::XMFLOAT2 stagesize;
	DirectX::SimpleMath::Vector3 hitbackrotation = { 0,0,0 };//攻撃に当たった時のノックバックする向き
	std::vector<Bullet*> m_bullet;
	Pole* m_weapon;
	DirectX::SimpleMath::Vector3 m_ta_pos; // 突進などの目標点
	Vibration m_vib; // 振動用のクラス

	const float gravity = -0.007f;

	void LookAt(DirectX::SimpleMath::Vector3);
	void Move();
	void AttackUpdate();
	void StunUpdate();
	void SetArrow();
	void Stun(std::optional<DirectX::SimpleMath::Vector3> knockbackDir = std::nullopt ); // 行動不能状態にする

	void DebugBossStatus();

};

