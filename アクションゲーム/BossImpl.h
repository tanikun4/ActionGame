#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Boss.h"
#include "AnimStruct.h"

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
	void Stun(std::optional<DirectX::SimpleMath::Vector3> knockbackDir = std::nullopt); // 行動不能状態にする
	void SetDEF(int _def) { def = _def; }

	void ShotBullet();
	bool GetLive();
	int GetHP();
	
	Pole* GetWeapon();

	void OnHit(Pole* pole);
	void OnHit(Bullet* bu);
	void OnHit(TestCube* cube);
	void OnHit(Projectile* pr);
private:
	// 所有者とカメラ参照
	Boss* m_Owner = nullptr;
	Camera* m_Camera = nullptr;
	enum State {
		NORMAL = 0, //通常状態
		ATTACK,		// 攻撃中
		STUN,		// 行動不能、ジャストガードされると移行する。
	};

	enum Attack_Kind {
		NONE = -1, //攻撃なし
		SWING = 0,
		ROTATESWING,
		SWING_VERTICAL,
		MANY_THRUST,
		THREE_SWING,
		JUMP_SPINSLASH,
		JUMP_SPINSLASH_RUSH,
		SONICBOOM_SHOT,
		WRAPAROUND_THRUST,

		KIND_MAX
	};

	// 攻撃フェーズ
	enum class AttackPhase
	{
		ENTER,      // 攻撃開始直後（初期化）
		PREPARE,    // 構え・溜めなど
		ATTACK,     // 攻撃
		FOLLOW,     // 追撃・派生・急降下など
		RECOVER,    // 硬直・構え戻し
		END
	};

	AttackPhase m_attackPhase = AttackPhase::ENTER;

	const int projectile_max = 5;//飛び道具の最大数

	int hp = 50;
	int def = 0; //防御力、値分ダメージを減らす
	int m_stateframe = 0;//状態継続フレーム数
	bool inviFg = false;
	int invicount = 0;
	float move_speed = 0.25f;
	float rotate_speed = 0.01;
	int attack_kind = 0;//攻撃の種類
	int m_attackframe = 0;//攻撃時間
	int m_attackcount = 0;//攻撃回数のカウント
	int m_rand = 0;// 現在の乱数値
	bool notUpdate = false;//更新を止めるかどうか
	bool m_lookatFg = true;//プレイヤーへの追従をするかどうか
	bool m_rushFg = false;//突進しているかどうか
	float jumppower = 1.5f;//ジャンプ力

	bool m_slowFg = false;//動きが遅い状態か
	int slow_frame = 0;//遅くなっているフレーム数
	int slow_rate = 6;//元の速度の何分の1にするか

	DirectX::SimpleMath::Vector3 m_destrot;//回転の目標点
	DirectX::XMFLOAT2 stagesize;
	DirectX::SimpleMath::Vector3 hitbackrotation = { 0,0,0 };//攻撃に当たった時のノックバックする向き
	std::vector<Bullet*> m_bullet;
	std::vector<Projectile*> m_projectile;
	Pole* m_weapon;
	DirectX::SimpleMath::Vector3 m_ta_pos; // 突進などの目標点
	Vibration m_vib; // 振動用のクラス

	AngleAnim m_AngleAnim;// 角度アニメーション構造体
	ArcMoveAnim m_ArcAnim;// 円弧移動アニメーション構造体

	void LookAt(DirectX::SimpleMath::Vector3 ta_pos);
	void Move();
	void AttackUpdate();
	void StunUpdate();
	void Jump();
	void StateReset();//状態リセット

	
	// 飛び道具関連
	void SetProjectile();
	// 飛び道具構え
	void ProjectileCharge();
	void ProjectileCharge_VT();
	void ProjectileChargeMax();
	void ProjectileChargeMax_VT();
	// 飛び道具発射
	void ProjectileShot();

	void DebugBossStatus();

};

