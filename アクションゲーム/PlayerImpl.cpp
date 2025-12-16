#include "PlayerImpl.h"

#include "StaticMesh.h"
#include "utility.h"
#include "Collision.h"
#include "Game.h"
#include "Ground.h"
#include "Pole.h"
#include "Arrow.h"
#include "Bullet.h"
#include "Enemy.h"
#include "sound.h"
#include "Boss.h"
#include "WeaponManager.h"
#include "DebugUI.h"

#include "EffectManager.h"


#include <imgui.h>
#include <cmath>
#include <vector>

using namespace DirectX::SimpleMath;
using namespace std;

//Player::Implクラスの実装

// -------------------------
// コンストラクタ / デストラクタ
// -------------------------
Player::Impl::Impl(Camera* cam, Player* owner)
    : m_Owner(owner)
    , m_Camera(cam)
{
    m_arrow = nullptr;
}

Player::Impl::~Impl()
{
}

// -------------------------
// public関数
// -------------------------
void Player::Impl::Init() {
    m_Owner->GBInit(u8"assets/model/gorufu/GolfBall_v2.fbx");
    m_Owner->m_Position = Vector3(0.0f, 50.0f, 0.0f);
    m_pole = Game::GetInstance()->AddObject<Pole>();
    m_pole->SetPl(true);
    hp = 9;
    framecount = 30;
    m_pole->SetAtk(3);

    //丸影の大きさ調整
    m_Owner->m_Shadow->SetBaseScale(15 * m_Owner->m_Scale.x);

    // デバッグ関数の登録
    DebugUI::RedistDebugFunction([this]() {
        DebugPlayerStatus();
        DebugWeaponStatus();
        DebugEffectPlay();
		m_pole->DebugPoleStatus();
        });
}

void Player::Impl::Update() {
    if (hp <= 0) return; // 死亡していたら更新しない

	if (demoMode) UpdateDemo(); // デモモード中はデモ更新

    // 状態ごとの処理
    switch (m_Owner->m_State) {
    case NORMAL:
        m_Owner->m_Velocity_f = 0.0f; // 移動速度をリセット
        UpdateNormal();
        break;
    case ATTACK:
        if (m_Owner->is_GROUND) m_Owner->m_Velocity_f = 0.0f;
        UpdateAttack();
        break;
    case SHOT:
        // 射撃は一旦削除
        break;
    case DAMAGE:
        UpdateDamage();
        break;
    case DODGE:
        UpdateDodge();
        break;
    case COUNTER:
        UpdateCounter();
        break;
    default:
        break;
    }

    if (m_Owner->m_State != DODGE) {
        if (rollcount < rollcooldown) ++rollcount;

        if (inviFg) {
            if (invicount < 60) ++invicount;
            else {
                inviFg = false;
                m_Owner->SetColor({ 1,1,1,1 });
                invicount = 0;
            }
        }

        if (framecount < 60) ++framecount;
        if (GuardFg) ++guardcount;
    }

    // 下に落ちた時はダメージを受けてリスポーン
    if (m_Owner->m_Position.y < -100) {
        hp -= 1;
        m_Owner->m_Position = Vector3(0.0f, 50.0f, 0.0f);
        m_Owner->m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
    }

    if (m_Owner->is_GROUND) is_JUMP = false;

    m_Owner->m_Rotation.y = m_Owner->m_ForwardRotation.y;
    m_Owner->GBUpdate();

    if (m_pole)
        m_pole->Update(m_Owner->m_Position, m_Owner->radius, m_Owner->m_Rotation, 1.7f);

}

void Player::Impl::Uninit() {
    m_pole = nullptr;
}

int Player::Impl::GetHP() {
    return hp;
}

Pole* Player::Impl::GetWeapon() {
    return m_pole;
}

// -------------------------
// OnHit 系
// -------------------------

//ボスと当たった場合
void Player::Impl::OnHit(Boss* bo) {
    Damage(1);
}

//近接武器と当たった場合
void Player::Impl::OnHit(Pole* po) {
    const int damage = 2;
    if (po->GetPl()) return;
    if (RollFg && rollcount < 5 && po->GetSwingTime() < 5) { Counter(); return; } // 回避の初めに攻撃を受けたらカウンター
    if (GuardFg && guardcount < justguardframe && po->GetSwingTime() < 10) { Counter(); return; } // ガードの初めに攻撃を受けたらカウンター
    Damage(damage);
}

//弾と当たった場合
void Player::Impl::OnHit(Bullet* bu) {
    if (bu->GetPl()) return;
    if (GuardFg && guardcount < justguardframe) { Counter(); return; }
    const int damage = 2;
    Damage(damage);
}

//ブロックと当たった場合
void Player::Impl::OnHit(TestCube* cube) {
    auto& col = m_Owner->GetLastCollision();
    float vn = m_Owner->m_Velocity.Dot(col.normal);

    if (vn < 0.0f) {
        m_Owner->m_Velocity -= col.normal * vn;

        if (col.normal.y > 0.6f) {
            // 床
            m_Owner->m_Velocity.y = 0.0f;
            m_Owner->m_Position.y = m_Owner->m_oldPos.y;
            m_Owner->is_GROUND = true;
        }
        else if (col.normal.y < -0.6f) {
            // 天井
            m_Owner->m_Velocity.y = 0.0f;
        }
        else {
            // 壁
            m_Owner->m_Velocity.x = 0.0f;
            m_Owner->m_Velocity.z = 0.0f;
            m_Owner->m_Position = m_Owner->m_oldPos;
        }
    }
}


// -------------------------
// ImGui デバッグ関数
// -------------------------

// 武器のオフセット調整
void Player::Impl::DebugWeaponStatus() {
    ImGui::Begin("WeaponStatus");

    static Vector3 weapon_offset{};
    ImGui::SliderFloat3("WeaponOffset", &weapon_offset.x, -10.0f, 10.0f);

    if (ImGui::Button("Reset OffSet"))
        weapon_offset = Vector3(0, 0, 0);

    if (ImGui::Button("Set Int"))
        weapon_offset = Vector3((int)weapon_offset.x, (int)weapon_offset.y, (int)weapon_offset.z);

    static Vector3 weapon_angle{};
    ImGui::SliderFloat3("WeaponAngle", &weapon_angle.x, -PI, PI);

    if (ImGui::Button("Reset Angle"))
        weapon_angle = Vector3(0, 0, 0);

    if (m_pole) {
        m_pole->SetOffsetDebug(weapon_offset);
        m_pole->SetAngleDebug(weapon_angle);
    }

    ImGui::End();
}


// プレイヤーj状態の操作
void Player::Impl::DebugPlayerStatus() {
    ImGui::Begin("PlayerStatus");

    ImGui::SliderFloat3("Rotation", &m_Owner->m_Rotation.x, -PI, PI);

    if (ImGui::Button("HP MAX"))
        hp = 9;

    if (ImGui::Button("HP ZERO"))
        hp = 0;

    static bool select;
    ImGui::Checkbox("Invisible", &select);

    if (select) {
        inviFg = true;
        m_Owner->SetColor({ 0,0,1,0.5f });
    }

	ImGui::Checkbox("DEMOMODE", &demoMode);

    ImGui::End();
}

void Player::Impl::DebugEffectPlay() {
    ImGui::Begin("EffectPlay");

	static int debug_effect_type = 0;
    ImGui::SliderInt("ID", &debug_effect_type, 0, EFFECT_MAX - 1);

    static int debug_effect_life = 1;
    ImGui::SliderInt("Life", &debug_effect_life, 1, 60);

    static Vector3 offset_pos{};
	ImGui::SliderFloat3("OffsetPosition", &offset_pos.x, -50.0f, 50.0f);

    static Vector3 offset_rotation{};
    ImGui::SliderFloat3("OffsetRotation", &offset_rotation.x, -PI, PI);

    static Vector3 offset_scale = {1,1,1};
    ImGui::SliderFloat3("OffsetScale", &offset_scale.x, -10.0f, 10.0f);

    if(ImGui::Button("Reset Offsets")) {
        offset_pos = Vector3(0,0,0);
        offset_rotation = Vector3(0,0,0);
        offset_scale = Vector3(1,1,1);
	}

    //エフェクトパラメーター構造体作成
    static EffectParams debug_param;
    debug_param.pos = m_Owner->m_Position + offset_pos;
	debug_param.rot = offset_rotation;
    debug_param.scale = offset_scale;
    debug_param.maxLife = debug_effect_life;

    // エフェクト再生
    if (ImGui::Button("Play Effect")) {
        EffectManager::GetInstance()->Play(debug_effect_type,debug_param);
    }

    ImGui::End();
}


// キー入力による移動
void Player::Impl::Move() {
    float dir = -1.0f;

	// デモモード中はデモ用の移動方向を使用
    if(demoMode) dir = m_demoParam.demoMoveDir;
    else dir = SetMoveDirection();

    if (dir >= 0.0f) {
        m_Owner->m_ForwardRotation.y = dir + m_Owner->m_Camera->GetCameraDirection().x;
        m_Owner->m_Velocity_f = speed;		
		m_Owner->m_Rotation.x += speed * 0.1f;//回転、zだとドリルみたいになる。そういう突進技もありかも。
		if (m_Owner->m_Rotation.x > PI * 2) m_Owner->m_Rotation.x -= PI * 2;//回転リセット、値が大きくなりすぎないように

        if (moveframe > 10 && m_Owner->is_GROUND) {
            // 土煙エフェクト再生
            EffectParams   param;
            param.pos = m_Owner->m_Position;
            param.scale = m_Owner->m_Scale * 10;
            param.maxLife = 30;
            EffectManager::GetInstance()->Play(EFFECT_TUTIKEMURI, param);
			moveframe = 0;
        }

		moveframe++;
    }
    else {
        m_Owner->m_Velocity_f = 0.0f;
    }

}

// 移動方向の設定
float Player::Impl::SetMoveDirection() {
    bool w = Input::GetKeyPress(VK_W);
    bool s = Input::GetKeyPress(VK_S);
    bool a = Input::GetKeyPress(VK_A);
    bool d = Input::GetKeyPress(VK_D);

    if (w && a) return 3.0f * PI / 4.0f; // 前左
    if (w && d) return 5.0f * PI / 4.0f; // 前右
    if (s && a) return PI / 4.0f; // 後ろ左
    if (s && d) return 7.0f * PI / 4.0f; // 後ろ右
    if (w)      return PI; // 前
    if (s)      return 0.0f; // 後ろ
    if (a)      return PI / 2.0f; // 左
    if (d)      return 3.0f * PI / 2.0f; // 右

    return -1.0f;
}

// 回避処理
void Player::Impl::DodgeRoll() {
	if (demoMode) return;//デモ中は入力を受け付けない
    
    if (Input::GetKeyTrigger(VK_J) && rollcount >= rollcooldown) {
      m_Owner->m_State = DODGE;
      rollcount = 0;
      inviFg = true;
      RollFg = true;
      m_Owner->SetColor({ 0,0,1,0.5f });        
    }
}

// 攻撃処理
void Player::Impl::Attack() {
    
	//デモ中は入力を受け付けない
    if (demoMode)  return;
    
    if (Input::GetKeyTrigger(VK_K) && !GuardFg) {
        if (m_pole) m_pole->Swing();
        m_Owner->m_State = ATTACK;
        Sound::GetInstance()->Play(SOUND_SE_SWING);
    }
}

// ジャンプ処理
void Player::Impl::Jump() {
    if (demoMode) return;//デモ中は入力を受け付けない

    if (Input::GetKeyTrigger(VK_L) && !is_JUMP) {
        m_Owner->m_Velocity.y = 2.0f;
        m_Owner->is_GROUND = false;
        is_JUMP = true;
        m_Owner->m_Position.y += 0.1f;
    }
}


void Player::Impl::Charge() {
    if (!m_arrow) {
        vector<Bullet*> bullet = Game::GetInstance()->GetObjects<Bullet>();
        for (auto& bu : bullet) {
            if (bu->GetState() == 0) {
                m_arrow = bu;
                break;
            }
        }
        if (!m_arrow) return;
    }
    m_arrow->PCharge(m_Owner->m_Rotation.y, m_Owner->radius, m_Owner->m_Position);
}

void Player::Impl::Shot() {
    if (!m_arrow) {
        vector<Bullet*> bullet = Game::GetInstance()->GetObjects<Bullet>();
        for (auto& bu : bullet) {
            if (bu->GetState() == 0) {
                m_arrow = bu;
                break;
            }
        }
        if (!m_arrow) return;
    }
    m_arrow->PShot(m_Owner->m_Rotation.y, m_Owner->radius, m_Owner->m_Position);
    m_arrow = nullptr;
}

void Player::Impl::Damage(int atk) {
    if (inviFg == false) {
        if (GuardFg) {
            if (atk > 1) atk = atk / 2;
        }
        hp -= atk;
        m_Owner->m_State = DAMAGE;
        framecount = 0;
        inviFg = true;

        m_Owner->SetColor(Vector4(1, 1, 0, 0.5f));

        if (GuardFg) {
			EffectParams param;

            //エフェクトパラメーター構造体設定
			Vector3 pos = m_Owner->m_Position + (m_Owner->radius * m_Owner->AngleToForward(m_Owner->m_ForwardRotation));
            param.pos = EffectManager::ToCameraEffectPos(pos, m_Owner->radius * m_Owner->m_Scale.x);
            param.scale = m_Owner->m_Scale * 15;
            param.maxLife = 15;
			EffectManager::GetInstance()->Play(EFFECT_SPARK, param);
            Sound::GetInstance()->Play(SOUND_SE_PLAYERGUARD);
        }
        else {
            m_pole->SwingEnd();
            Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
        }
    }
}

void Player::Impl::Guard() {
	if (demoMode) return;//デモ中は入力を受け付けない

    if (Input::GetKeyTrigger(VK_I)) {
        GuardFg = true;
        guardcount = 0;
        speed = 0.1f;
        if (m_pole) m_pole->GuardStart();
    }
    if (Input::GetKeyRelease(VK_I)) {
        GuardFg = false;
        speed = 1;
        if (m_pole) m_pole->GuardEnd();
    }
}

//カウンター処理
void Player::Impl::Counter() {
    auto bosses = Game::GetInstance()->GetObjects<Boss>();
    if (!bosses.empty()) {
        Boss* boss = bosses[0];
        LookAt(boss->GetPosition());
    }
    rollcount = 0;
    m_Owner->m_State = COUNTER;
    speed = 1.0f;
    RollFg = false;
    GuardFg = false;
    inviFg = true;
    if (m_pole) m_pole->GuardEnd();
    m_Owner->SetColor({ 0,0,1,0.5f });
    Sound::GetInstance()->Play(SOUND_SE_PLAYERJUSTGUARD);
}

// 指定座標の方向に向く
void Player::Impl::LookAt(Vector3 ta_pos) {
    m_Owner->m_ForwardRotation.y = atan2f((ta_pos.x - m_Owner->m_Position.x), (ta_pos.z - m_Owner->m_Position.z));
    m_ta_pos = ta_pos;
}

// -------------------------
// 状態別Update関数
// -------------------------

//通常時
void Player::Impl::UpdateNormal() {
    Move();
    Attack();
    Guard();
    Jump();
    DodgeRoll();
}


//攻撃中
void Player::Impl::UpdateAttack() {
    if (m_pole) {
        if (m_pole->GetSwingTime() > 18) {
            m_Owner->m_State = NORMAL;
            m_pole->SwingEnd();
        }
    }
    else {
        m_Owner->m_State = NORMAL;
    }
}

//ダメージ中
void Player::Impl::UpdateDamage() {
    m_Owner->m_Velocity_f = speed * -1;
    Guard();
    if (framecount > 10) {
        m_Owner->m_State = NORMAL;
    }
}

//回避中
void Player::Impl::UpdateDodge() {
    m_Owner->m_Velocity_f = speed * 2.0f;
    ++rollcount;
    if (rollcount % 3 == 0) {
        // 土煙エフェクト再生
        EffectParams   param;
        param.pos = m_Owner->m_Position;
        param.scale = m_Owner->m_Scale * 20;
        param.maxLife = 30;
        EffectManager::GetInstance()->Play(EFFECT_TUTIKEMURI, param);
    }
    if (rollcount > 10) {
        RollFg = false;
        m_Owner->m_State = NORMAL;
        inviFg = false;
        rollcount = 0;
        m_Owner->SetColor({ 1,1,1,1 });
    }
}

//カウンター攻撃中
void Player::Impl::UpdateCounter() {

	//常に3倍の速度で移動
    //攻撃後も移動し続けるので、切り抜ける形になってかっこよくなった。
    m_Owner->m_Velocity_f = speed * 3;

    if (fabs(m_Owner->m_Position.x - m_ta_pos.x) < m_Owner->radius * 5 &&
        fabs(m_Owner->m_Position.z - m_ta_pos.z) < m_Owner->radius * 5) {

		//暫定的なカウンター攻撃処理
        if (m_pole->GetSwingTime() <= 0) {
            m_pole->SetAtk(6);
            if (m_pole) m_pole->Swing_Vertical();
        }
    }
    else {
        auto bosses = Game::GetInstance()->GetObjects<Boss>();
        if (!bosses.empty()) {
            Boss* boss = bosses[0];
            LookAt(boss->GetPosition());
        }
    }
	// カウンター攻撃終了判定
    if (m_pole->GetSwingTime() > 10) {
        m_pole->SwingEnd();
        m_pole->SetAtk(3);
        inviFg = false;
        m_Owner->SetColor({ 1,1,1,1 });
	    m_Owner->m_State = NORMAL;
		Sound::GetInstance()->Play(SOUND_SE_SWINGVERTICAL);
    }
}

//デモ中の更新処理

void Player::Impl::UpdateDemo()
{
    // 移動
    if (--m_demoParam.demoMoveframe <= 0)
    {
        // 次の間隔（ランダム）
        m_demoParam.demoMoveframe = (rand() % 90) + 30;

        // 移動方向ランダム
        static const float dirs[] = {
             3.0f * PI / 4.0f, // 前左
             5.0f * PI / 4.0f, // 前右
             PI / 4.0f, // 後ろ左
             7.0f * PI / 4.0f, // 後ろ右
             PI, // 前
             0.0f, // 後ろ
             PI / 2.0f, // 左
             3.0f * PI / 2.0f // 右
        };
        m_demoParam.demoMoveDir = dirs[rand() % 8];
    }

    // 攻撃
    if (--m_demoParam.demoAttackframe <= 0)
    {
        m_demoParam.demoAttackframe = (rand() % 180) + 60;

        if (m_pole && m_Owner->m_State == NORMAL)
        {
            m_pole->Swing();
            m_Owner->m_State = ATTACK;
        }
    }
}

