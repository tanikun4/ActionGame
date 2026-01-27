#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Player.h"
#include "AnimStruct.h"
#include "UIStruct.h"

class Camera;
class Player;
class Pole;
class Boss;
class Bullet;
class TestCube;


//Player::Impl クラス、これとcppにPrivateの実装を記述することで、ビルド時間を短縮する。
class Player::Impl
{
public:
    Impl(Camera* cam, Player* owner);
    ~Impl();

    void Init();
    void Update();
	void Draw();
    void Uninit();
	void SetGauge();//ゲージ初期化用、ゲームシーンでのみ呼び出す

    void SetDemoMode(const bool _demomode) { demoMode = _demomode; }

    int GetHP();
    Pole* GetWeapon();
    std::vector<Texture2D*> GetGauge();

    void OnHit(Boss* bo);
    void OnHit(Pole* po);
    void OnHit(Bullet* bu);
    void OnHit(Projectile* pr);
    void OnHit(TestCube* cube);

private:
    // 所有者とカメラ参照
    Player* m_Owner = nullptr;
    Camera* m_Camera = nullptr;

    enum STATE {
        NORMAL = 0,
        ATTACK,
        SHOT,
        DAMAGE,
        DODGE,
        COUNTER,
		PARRY,

    };

    enum ATTACK_KIND {
        NONE = -1,
        SWING = 0,
        SPINSLASH,
        SPINSLASH_VT,

        KIND_MAX,
	}; 

    enum COMBO_KIND {
        COMBO_NONE = -1,
        COMBO_1 = 0,
        COMBO_2,
        COMBO_3,

		COMBO_MAX,
	};

	// デモ用パラメータ構造体
    struct DemoParam {
        int demoMoveframe = 120;
        int demoAttackframe = 300;

        float demoMoveDir = -1.0f;
	};

	bool demoMode = false; // デモモード
	DemoParam m_demoParam; // デモ用パラメータ

    int hp = 20;
	const int maxhp = 20;
	int atk = 3; // 攻撃力
    int framecount = 0;
    int invicount = 0; // 無敵時間カウント
	int maxinvicount = 60; // 無敵時間最大値
    int rollcooldown = 30; // 回避クールタイム
    int rollcount = 0; // 回避関連カウント
    int guardcount = 0; // ガード時間カウント

	int moveframe = 0; // 移動フレームカウント
	int attackframe = 0; // 攻撃フレームカウント
	int maxattackframe = 30; // 攻撃最大フレーム
	//int swing_time = 18; // 振り攻撃時間
	
    int attackcombo = 0; // 攻撃コンボ数

    bool inviFg = false; // 無敵
    bool GuardFg = false; // ガード
    bool RollFg = false; // 回避
	bool parryFg = false; // ジャストガード中フラグ
    bool is_JUMP = false; // ジャンプフラグ
	bool is_InputMove = true; // 移動入力可能フラグ
	int m_attackkind = NONE; // 攻撃種類

    float speed = 1.0f; // 移動速度

    const int justguardframe = 10; // ジャストガード有効フレーム

    DirectX::SimpleMath::Vector3 m_ta_pos; // 目標点
    Bullet* m_arrow = nullptr;
    Pole* m_weapon = nullptr;
	Object* m_target = nullptr; // 攻撃対象
	AngleAnim m_Anim; // アニメーション用構造体
    Gauge m_hp_gauge; //  HPゲージ用構造体

    // 各種行動関数
    void Move();
    float SetMoveDirection();
    void Attack();
    void Charge();
    void Shot();
    void Damage(int atk);
    void DodgeRoll();
    void Guard();
    void Counter();
    void Jump();
    void LookAt(DirectX::SimpleMath::Vector3 ta_pos);
    void BossStan();// ボスの動きを止める
	void Parry();//ジャストガード処理

	// 回転斬り攻撃開始 
	void SpinAttack(const int& t = 24, const int& attack_t = 18, const float& accel = 0);//全体フレーム、攻撃有効フレーム、加速度
	// 縦回転斬り攻撃開始
	void SpinAttack_Vertical(int t = 30, int attack_t = 20, float accel = 0);//全体フレーム、攻撃有効フレーム、加速度
    //通常攻撃開始
	void SwingAttack(int _swingtime = 18,int _atk = 3);
	// 共通Update処理関数
    void UpdateCommon();

	// 状態別Update関数
    void UpdateNormal();
    void UpdateAttack();
    void UpdateDamage();
    void UpdateDodge();
    void UpdateCounter();
	void UpdateParry();

	void UpdateDemo();

    void DebugWeaponStatus();
    void DebugPlayerStatus();
    void DebugEffectPlay();
    void DebugParticlePlay();
	void DebugHPGauge();
};
