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
	void Uninit();
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
		MANY_THRUST_LOOKAT,
		THREE_SWING,
		JUMP_SPINSLASH,
		JUMP_SPINSLASH_RUSH,
		SONICBOOM_SHOT,
		WRAPAROUND_THRUST,
		JUMP_SPINSLASH_SHOT,
		CROSS_SHOT,
		ROTATESWING_FIBONACCI,
		ALTEREGO_SHOT,

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

	// 突き攻撃の位置
	enum class ThrustType
	{
		Center,
		Left,
		Right
	};

	struct ThrustStep
	{
		ThrustType type;
		int delay;
	};

	static constexpr ThrustStep ComboThrust[] =
	{
		{ ThrustType::Center, 4 },
		{ ThrustType::Left,   4 },
		{ ThrustType::Right,  4 },
	};

	AttackPhase m_attackPhase = AttackPhase::ENTER;

	const int projectile_max = 5;//飛び道具の最大数

	int hp = 50;
	int def = 0; //防御力、値分ダメージを減らす
	int m_stateframe = 0;//状態継続フレーム数
	bool inviFg = false;
	int invicount = 0;
	float move_speed = 0.25f;
	float m_rotatespeed = 0.01;
	int attack_kind = 0;//攻撃の種類
	int m_attackframe = 0;//攻撃時間
	int m_attackcount = 0;//攻撃回数のカウント
	int m_rand = 0;// 現在の乱数値
	bool notUpdate = false;//更新を止めるかどうか
	bool m_lookatFg = true;//プレイヤーへの追従をするかどうか
	bool m_rushFg = false;//突進しているかどうか
	bool m_spinFg = false;//回転しているかどうか
	float jumppower = 1.5f;//ジャンプ力
	//const float delta60f = 1.0f / 60.0f;// 60fps換算用

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
	DirectX::SimpleMath::Vector3 m_startpos; // 移動開始位置
	Vibration m_vib; // 振動用のクラス

	AngleAnim m_AngleAnim;// 角度アニメーション構造体
	ArcMoveAnim m_ArcAnim;// 円弧移動アニメーション構造体
	FibonacciAnim m_FiboAnim;// 黄金螺旋移動アニメーション構造体

	void LookAt(DirectX::SimpleMath::Vector3 ta_pos);
	void Move();
	void AttackUpdate();
	void StunUpdate();
	void Jump(float _power = 1.5f);
	void StateReset();//状態リセット

	void Thrust(ThrustType type);
	bool ManyThrust(int maxcount);

	// 行動パターン別関数
	void RotateSwing();
	void SwingVertical();
	void ManyThrustLookAt();
	void ThreeSwing();
	void JumpSpinSlash();
	void JumpSpinSlashRush();
	void SonicBoomShot();
	void WrapAroundThrust();
	void JumpSpinSlashShot();
	void CrossShot();
	void RotateSwingFibonacci();
	void AlterEgoShot();
	
	// 飛び道具関連
	void SetProjectile();
	// 飛び道具構え
	// offset: 発射位置のオフセット、_angle: オフセットの方向指定。offsetが0のときは持ち主に追従する
	void ProjectileCharge(float offset = 0, float _angle = 0);
	void ProjectileCharge_VT(float offset = 0, float _angle = 0);
	void ProjectileChargeMax(float offset = 0, float _angle = 0);
	void ProjectileChargeMax_VT(float offset = 0,float _angle = 0);
	// 飛び道具発射
	void ProjectileShot();
	void ProjectileShot_All();

	void DebugBossStatus();

};

