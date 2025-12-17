#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Player.h"

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
    void Uninit();

    void SetDemoMode(bool _demomode) { demoMode = _demomode; }

    int GetHP();
    Pole* GetWeapon();

    void OnHit(Boss* bo);
    void OnHit(Pole* po);
    void OnHit(Bullet* bu);
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
        COUNTER
    };

	// デモ用パラメータ構造体
    struct DemoParam {
        int demoMoveframe = 120;
        int demoAttackframe = 300;

        float demoMoveDir = -1.0f;
	};

	bool demoMode = false; // デモモード
	DemoParam m_demoParam; // デモ用パラメータ

    int hp = 9;
    int framecount = 0;
    int invicount = 0; // 無敵時間カウント
    int rollcooldown = 30; // 回避クールタイム
    int rollcount = 0; // 回避関連カウント
    int guardcount = 0; // ガード時間カウント

	int moveframe = 0; // 移動フレームカウント

    bool inviFg = false; // 無敵
    bool GuardFg = false; // ガード
    bool RollFg = false; // 回避
    bool is_JUMP = false; // ジャンプフラグ

    float speed = 1.0f; // 移動速度

    int justguardframe = 30; // ジャストガード有効フレーム

    DirectX::SimpleMath::Vector3 m_ta_pos; // 目標点
    Bullet* m_arrow = nullptr;
    Pole* m_pole = nullptr;

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


    void UpdateNormal();
    void UpdateAttack();
    void UpdateDamage();
    void UpdateDodge();
    void UpdateCounter();

	void UpdateDemo();

    void DebugWeaponStatus();
    void DebugPlayerStatus();
    void DebugEffectPlay();
};
