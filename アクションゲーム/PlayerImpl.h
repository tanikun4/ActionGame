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

    int hp = 9;
    int flamecount = 0;
    int invicount = 0; // 無敵時間カウント
    int rollcooldown = 30; // 回避クールタイム
    int rollcount = 0; // 回避関連カウント
    int guardcount = 0; // ガード時間カウント

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

    void DebugWeaponStatus();
    void DebugPlayerStatus();
    void DebugEffectPlay();
};
