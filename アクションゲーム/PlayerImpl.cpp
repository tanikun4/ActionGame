#include "PlayerImpl.h"

#include "StaticMesh.h"
#include "utility.h"
#include "Collision.h"
#include "Game.h"
#include "Ground.h"
#include "Sword.h"
#include "Projectile.h"
#include "Bullet.h"
#include "Enemy.h"
#include "sound.h"
#include "Boss.h"
#include "WeaponManager.h"
#include "DebugUI.h"

#include "EffectManager.h"
#include "ActionInput.h"

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
    m_weapon = Game::GetInstance()->AddObject<Sword>();
    m_weapon->SetOwner(m_Owner);
}

Player::Impl::~Impl()
{
}

// -------------------------
// public関数
// -------------------------
void Player::Impl::Init() {
    m_Owner->BallInit(u8"assets/model/Character/player.fbx");
    m_Owner->m_Position = Vector3(0.0f, 50.0f, 0.0f);
    m_weapon->SetPl(true);
    hp = maxhp;
    framecount = 30;
    m_weapon->SetAtk(atk);

    // 武器の軌跡色をセット
    m_weapon->SetTrailColor({ 0,0,1,1 });
    //丸影の大きさ調整
    m_Owner->m_Shadow->SetBaseScale(15 * m_Owner->m_Scale.x);

    // デバッグ関数の登録
    DebugUI::RedistDebugFunction([this]() {
        DebugPlayerStatus();
        DebugWeaponStatus();
        DebugEffectPlay();
        DebugParticlePlay();
		m_weapon->DebugPoleStatus();
        });
}

void Player::Impl::Update() {
    if (hp <= 0) return; // 死亡していたら更新しない

	if (demoMode) UpdateDemo(); // デモモード中はデモ更新

    // 状態ごとの処理
    switch (m_Owner->m_State) {
    case NORMAL:
        UpdateNormal();
        break;
    case ATTACK:
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
    case PARRY:
        UpdateParry();
        break;
    default:
        break;

    }
	UpdateCommon();
}

void Player::Impl::Draw() {
	if (hp <= 0) return; // 死亡していたら描画しない
    m_Owner->BallDraw();
}

void Player::Impl::Uninit() {
    m_weapon->SetOwner(nullptr);
    m_weapon = nullptr;
}

void Player::Impl::SetGauge() {
    m_hp_gauge.Init({ -250 ,-325, 0 }, { 500,50, 0 });
}

int Player::Impl::GetHP() {
    return hp;
}

Sword* Player::Impl::GetWeapon() {
    return m_weapon;
}

vector<Texture2D*> Player::Impl::GetGauge() {
	vector<Texture2D*> gauge;
    gauge.emplace_back(m_hp_gauge.GetGauge(0));
    gauge.emplace_back(m_hp_gauge.GetGauge(1));
    return gauge;
}

// -------------------------
// OnHit 系
// -------------------------

//敵と当たった場合
void Player::Impl::OnHit(Enemy* en) {
    Damage(1);
}

//ボスと当たった場合
void Player::Impl::OnHit(Boss* bo) {
    Damage(1);
}

//近接武器と当たった場合
void Player::Impl::OnHit(Sword* po) {
    if (po->GetPl()) return;
     // ジャストガード成功で相手を行動不能にする
    if (GuardFg && guardcount <= justguardframe) { 
        po->GetOwner()->Stun();
		m_target = po->GetOwner();
        Parry(); 
        return; 
    }
    Damage(2);
}
		//Counter();
    //if(RollFg && rollcount < 5 && po->GetAttackTime() < 10) { Counter(); return; } // 回避の初めに攻撃を受けたらカウンター
    //if (GuardFg && guardcount < justguardframe && po->GetAttackTime() < 10) { Counter(); return; } // ガードの初めに攻撃を受けたらカウンター

//弾と当たった場合
void Player::Impl::OnHit(Bullet* bu) {
    if (bu->GetPl()) return;
    if (GuardFg && guardcount <= justguardframe) { Counter(); return; }
    Damage(bu->GetAtk());
}

//飛び道具と当たった場合
void Player::Impl::OnHit(Projectile* pr) {
    if (pr->GetPl()) return;
    if (!pr->GetAtkFg()) return;
	// ジャストガードに成功したら反射する、ジャストガードモーション中でも反射する
    if (GuardFg && guardcount <= justguardframe) {
        m_target = pr->GetOwner();
        Parry();
    }
    if (parryFg) {
        pr->Reflect(true);
        return;
    }

    Damage(pr->GetAtk());
}

//ブロックと当たった場合
void Player::Impl::OnHit(TestCube* cube) {
    auto& col = m_Owner->GetLastCollision();
    float vn = m_Owner->m_Velocity.Dot(col.normal);

    if (vn >= 0.0f) { return; }

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
        //m_Owner->m_Velocity.x = 0.0f;
        //m_Owner->m_Velocity.z = 0.0f;
        m_Owner->m_Position.x = m_Owner->m_oldPos.x;
        m_Owner->m_Position.z = m_Owner->m_oldPos.z;

        if(demoMode) {
            // デモ中は壁にぶつかったら方向転換
            m_demoParam.demoMoveDir += PI;
            if (m_demoParam.demoMoveDir > PI * 2) m_demoParam.demoMoveDir -= PI * 2;
		}
    }
    
}


// -------------------------
// ImGui デバッグ関数
// -------------------------

// 武器のステータス操作
void Player::Impl::DebugWeaponStatus() {
    ImGui::Begin("WeaponStatus");

    static Vector3 weapon_offset{};
    ImGui::SliderFloat3("WeaponOffset", &weapon_offset.x, -10.0f, 10.0f);

	static float trailsize = 1.0f;
    ImGui::SliderFloat("TrailSize", &trailsize, 0.0f, 10.0f);

    if (ImGui::Button("Reset OffSet"))
        weapon_offset = Vector3(0, 0, 0);

    static Vector3 debug_scale{3,3,3};
    ImGui::SliderFloat3("WeaponScale", &debug_scale.x, 0.0f, 10.0f);

    if (ImGui::Button("Reset Scale"))
        debug_scale = Vector3(3, 3, 3);

    if (ImGui::Button("Set Int"))
        debug_scale = Vector3((int)debug_scale.x, (int)debug_scale.y, (int)debug_scale.z);

    static Vector3 weapon_angle{};
    ImGui::SliderFloat3("WeaponAngle", &weapon_angle.x, -PI, PI);

    if (ImGui::Button("Reset Angle"))
        weapon_angle = Vector3(0, 0, 0);

    if (m_weapon) {
        m_weapon->SetOffsetDebug(weapon_offset);
        m_weapon->SetAngleDebug(weapon_angle);
        m_weapon->SetTrailSize(trailsize);
		m_weapon->SetScale(debug_scale);
    }

    ImGui::End();
}


// プレイヤー状態の操作
void Player::Impl::DebugPlayerStatus() {
    ImGui::Begin("PlayerStatus");

    ImGui::SliderFloat3("Rotation", &m_Owner->m_Rotation.x, -2 * PI, 2 * PI);
    if (ImGui::Button("Reset Rotation"))
    {
        m_Owner->m_Rotation.x = 0;
        m_Owner->m_Rotation.z = 0;
    }

	static float debug_scale = 1;
    ImGui::SliderFloat("Scale", &debug_scale, 0, 10);

    if (ImGui::Button("Reset Scale")) {
        debug_scale = 1;
    }

	m_Owner->m_Scale = Vector3(debug_scale, debug_scale, debug_scale);
	m_Owner->radius = m_Owner->base_radius * debug_scale;

    if (ImGui::Button("HP MAX"))
    {
        hp = maxhp;
        m_hp_gauge.ChangeGauge(hp, maxhp);
    }

    if (ImGui::Button("HP ZERO")) {
        hp = 0;
        m_hp_gauge.ChangeGauge(hp, maxhp);
    }

	static int slowtime = 300;

    ImGui::SliderInt("SLOW Time", &slowtime, 0,1000);

    if (ImGui::Button("SLOWMOTION"))
        Game::GetInstance()->SlowMotion(slowtime);

	static bool inviFg_Debug = false;
    ImGui::Checkbox("Invisible", &inviFg_Debug);

    if (inviFg_Debug) {
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

void Player::Impl::DebugParticlePlay() {
    ImGui::Begin("ParticlePlay");

    static int debug_effect_type = 1;
    ImGui::SliderInt("ID", &debug_effect_type, 0, EFFECT_MAX - 1);

    static Int2 debug_effect_life = Int2(1,1);
    ImGui::SliderInt2("LifeMinMax", &debug_effect_life.x, 1, 300);

	static int debug_count = 10;
	ImGui::SliderInt("Count", &debug_count, 1, 100);

	static int debug_interval = 60;
    ImGui::SliderInt("Interval", &debug_interval, 1, 300);

    static Vector3 offset_pos{5,0,0};
    ImGui::SliderFloat3("OffsetPosition", &offset_pos.x, -50.0f, 50.0f);

    static Vector3 range{5,0,0};
    ImGui::SliderFloat3("positionRange", &range.x, -50.0f, 50.0f);

    static Vector2 offset_rotation{};
    ImGui::SliderFloat2("OffsetRotationMinMax", &offset_rotation.x, -PI, PI);

    static Vector2 offset_scale = { 10,10};
    ImGui::SliderFloat2("OffsetScaleMinMax", &offset_scale.x, 0.0f, 100.0f);

    static Vector3 velocity_min = {0,0.5,0};
    ImGui::SliderFloat3("VelocityMin", &velocity_min.x, -50.0f, 50.0f);

    static Vector3 velocity_max = { 0,2,0 };
    ImGui::SliderFloat3("VelocityMax", &velocity_max.x, velocity_min.x, 50.0f);

    if (ImGui::Button("Reset Offsets")) {
        offset_pos = Vector3(0, 0, 0);
        offset_rotation = Vector2(0,0);
        offset_scale = Vector2(1, 1);
    }

    //エフェクトパラメーター構造体作成
    static ParticleEmitterParam2D debug_param;
    debug_param.pos = m_Owner->m_Position + offset_pos;
	debug_param.pos_range = range;

    debug_param.rotMin = offset_rotation.x;
    debug_param.rotMax = offset_rotation.y;

    debug_param.scaleMin = offset_scale.x;
    debug_param.scaleMax = offset_scale.y;

    debug_param.lifeMin = debug_effect_life.x;
    debug_param.lifeMax = debug_effect_life.y;

	debug_param.velocityMin = velocity_min;
	debug_param.velocityMax = velocity_max;

	debug_param.count = debug_count;
	debug_param.interval = debug_interval;

    ImGui::Checkbox("Loop", &debug_param.loop);
    ImGui::Checkbox("UI", &debug_param.UI);

    // エフェクト再生
    if (ImGui::Button("Play Particle")) {
        EffectManager::GetInstance()->Play(debug_effect_type, debug_param);
    }

    ImGui::End();
}

void Player::Impl::DebugHPGauge() {
    ImGui::Begin("HP Gauge");

	static Vector3 debug_gauge_pos = { 0,0,0 };
    ImGui::SliderFloat3("Position", &debug_gauge_pos.x, -1000.0f, 1000.0f);

    static Vector3 debug_gauge_scale = { 1,1,1 };
    ImGui::SliderFloat3("OffsetScale", &debug_gauge_scale.x, 0.0f, 1000.0f);

    if (ImGui::Button("Reset")) {
        debug_gauge_pos = { 0,0,0 };
        debug_gauge_scale = { 0,0,0 };
    }

	m_hp_gauge.SetPosScale(debug_gauge_pos, debug_gauge_scale);

    ImGui::End();
}

// キー入力による移動
void Player::Impl::Move() {
    float dir = -1.0f;

	// デモモード中はデモ用の移動方向を使用
    if (demoMode) dir = m_demoParam.demoMoveDir;
    else dir = ActionInput::GetInstance().GetMoveDirectionRad();

    if (dir >= 0.0f) {
        m_Owner->m_ForwardRotation.y = dir + m_Owner->m_Camera->GetCameraDirection().x;
        m_Owner->m_Velocity_f = speed;		
		//m_Owner->m_Rotation.x += speed * 0.1f;//回転、zだとドリルみたいになる。そういう突進技もありかも。
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

// 移動方向の設定、ActionInput側で指定するようになったので没
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
    
    if (ActionInput::GetInstance().IsTrigger(Action::Dodge) && rollcount >= rollcooldown) {
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
    if (!m_weapon) return;
    if (ActionInput::GetInstance().IsTrigger(Action::Attack) && !GuardFg) {
        // ダメージモーション中なら処理をしない
        if (m_Owner->m_State == DAMAGE) return;

		// ジャンプ中なら縦回転切り
        if (!m_Owner->is_GROUND) {
            SpinAttack_Vertical(12, 10, 0.3f);
            m_Owner->m_State = ATTACK;
            return;
        }
		// 構え開始
      m_weapon->Stance(30, StanceMode::NORMAL);
	  speed = 0.5f;  
    }
    if (ActionInput::GetInstance().IsRelease(Action::Attack) && !GuardFg) {
		m_weapon->StanceEnd();
        speed = 1.0f;
		// ダメージモーション中なら攻撃しない
        if (m_Owner->m_State == DAMAGE) return;

		// 構えきっていたら回転切り、そうでなければ通常攻撃
        if (m_weapon->GetMaxStance()) {
            SpinAttack(24,18,0.5f);
        }
        else {
			// 地上にいる場合のみ通常攻撃
            if (m_Owner->is_GROUND)
             SwingAttack();
        }
        m_Owner->m_State = ATTACK;
    }
}

// 通常攻撃開始
void Player::Impl::SwingAttack(int _swingtime,int _atk) {
    attackframe = 0;
    maxattackframe = 48;
    switch (attackcombo) {
    case COMBO_1:
        m_weapon->Swing(_swingtime,SwingMode::NORMAL);
        m_weapon->SetAtk(_atk);
		m_Anim.StartAbsolute({ 0,-PI * 0.3f,0 }, { 0,PI * 0.3f,0 }, _swingtime, 0.7f);
        break;
	case COMBO_2:
        m_weapon->Swing(_swingtime,SwingMode::RETURN);
        m_weapon->SetAtk(_atk);
        m_Anim.StartAbsolute({ 0,PI * 0.3f,0 }, { 0,-PI * 0.3f,0 }, _swingtime, 0.7f);
		break;
    case COMBO_3:
        m_weapon->Swing(_swingtime,SwingMode::VERTICAL);
        m_weapon->SetAtk(_atk + 1);
        m_Anim.StartAbsolute({ -PI * 0.3f,0,0 }, { PI * 0.1f,0,0 },_swingtime, 1.0f);
        break;
    }
	++attackcombo;
    if (attackcombo >= COMBO_MAX) attackcombo = 0;
    m_attackkind = SWING;
    Sound::GetInstance()->Play(SOUND_SE_SWING);
}

// 回転斬り攻撃開始
void Player::Impl::SpinAttack(const int& t, const int& attack_t , const float& accel) {
    m_weapon->AttackStart();
    m_weapon->SetAtk(atk * 2);
	m_attackkind = SPINSLASH;
    speed = 2.0f;
	maxattackframe = t;
	m_Anim.StartAbsolute({ 0,0,0 }, {0,PI * 2,0}, attack_t, accel);// 回転切り、絶対値参照で行う
    m_Owner->SetColor({ 0,0,1,0.5f });
	inviFg = true;
	invicount = 0;
    attackframe = 0;

    Sound::GetInstance()->Play(SOUND_SE_SWING);
}

// 縦回転斬り攻撃開始
void Player::Impl::SpinAttack_Vertical(int t, int attack_t, float accel) {
    m_weapon->AttackStart(true,true);
    m_weapon->SetAtk(atk + 1);
    m_attackkind = SPINSLASH_VT;
    maxattackframe = t;
    m_Anim.StartAbsolute({ 0,0,0 }, { PI * 2 ,0,0}, attack_t, accel);// 縦回転切り、絶対値参照で行う
    attackframe = 0;

    Sound::GetInstance()->Play(SOUND_SE_SWING);
}

// ジャンプ処理
void Player::Impl::Jump() {
    if (demoMode) return;//デモ中は入力を受け付けない

    if (ActionInput::GetInstance().IsTrigger(Action::Jump) && !is_JUMP) {
        m_Owner->m_Velocity.y = 1.3f;
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
    // 無敵状態なら処理を行わない
    if (inviFg) { return; }

	// 攻撃中なら攻撃終了
    if (m_Owner->m_State == ATTACK) {
        //m_Owner->m_State = NORMAL;
        m_weapon->AttackEnd();
        m_attackkind = NONE;
        m_Owner->m_Rotation.x = 0;
    }

    m_Owner->m_State = DAMAGE;
    framecount = 0;
    inviFg = true;

    m_Owner->SetColor(Vector4(1, 1, 0, 0.5f));

	if (demoMode) { m_weapon->SwingEnd(); return; }//デモ中はダメージを受けず、演出処理を終了

    // ガード中ならダメージを半減
    if (GuardFg) {
        if (atk > 1) atk = atk / 2;
        m_Camera->StartVibration(4.0f, PI * 0.5f, 4);

		// ガードエフェクト再生
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
        m_Camera->StartVibration(8.0f, PI * 0.5f, 6);

        m_weapon->SwingEnd();
        Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
    }

    hp -= atk;
	m_Owner->m_Scale = { 1.3f, 0.5f, 0.7f };// ダメージを受けたら少し変形する

    m_hp_gauge.ChangeGauge(hp, maxhp);
    
}

void Player::Impl::Guard() {
	if (demoMode) return;//デモ中は入力を受け付けない

    if (ActionInput::GetInstance().IsTrigger(Action::Guard)) {
        GuardFg = true;
        guardcount = 0;
        speed = 0.1f;
        if (m_weapon) m_weapon->GuardStart();
    }
    if (ActionInput::GetInstance().IsRelease(Action::Guard)) {
        GuardFg = false;
        speed = 1;
        if (m_weapon) m_weapon->GuardEnd();
    }
}

//カウンター処理
void Player::Impl::Counter() {
    
	m_ta_pos = m_target->GetPosition();
    LookAt(m_ta_pos);
    rollcount = 0;
    m_Owner->m_State = COUNTER;
    speed = 1.0f;
    RollFg = false;
    GuardFg = false;
    inviFg = true;
    if (m_weapon) m_weapon->GuardEnd();
    m_Owner->SetColor({ 0,0,1,0.5f });
    Sound::GetInstance()->Play(SOUND_SE_PLAYERJUSTGUARD);
}

void Player::Impl::BossStan() {
    // ボスの動きを止める
    auto bosses = Game::GetInstance()->GetObjects<Boss>();
    if (!bosses.empty()) {
        Boss* boss = bosses[0];
        boss->Stun();
    }
}

// パリィ処理
void Player::Impl::Parry() {
    //一段目のスイングを行う
    GuardFg = false;
	speed = 1.0f;
    if (m_weapon) m_weapon->GuardEnd();
	LookAt(m_target->GetPosition());
    //m_Owner->m_State = PARRY;
    //m_weapon->Swing_Parry();
    m_Owner->m_State = ATTACK;
	attackcombo = 0;
	SwingAttack(10,6);
	parryFg = true;
    // パリィ中は無敵にする
    inviFg = true;
    m_Owner->SetColor({ 0,0,1,0.5f });

    m_Owner->m_Scale = { 1.0f, 1.0f, 1.0f };// 元に戻す

    // パリィエフェクト再生
    EffectParams param;

    //エフェクトパラメーター構造体設定
    Vector3 pos = m_Owner->m_Position + (m_Owner->radius * m_Owner->AngleToForward(m_Owner->m_ForwardRotation));
    param.pos = EffectManager::ToCameraEffectPos(pos, m_Owner->radius * m_Owner->m_Scale.x * 3);
    param.scale = m_Owner->m_Scale * 30;
    param.maxLife = 30;
    EffectManager::GetInstance()->Play(EFFECT_SHOCKWAVE, param);

    Sound::GetInstance()->Play(SOUND_SE_PLAYERJUSTGUARD);

    m_Camera->StartVibration(10.0f, PI * 0.5f, 5);// カメラを揺らす

	Game::GetInstance()->SlowMotion(20); // スローモーション開始
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
    m_Owner->m_Velocity_f = 0.0f; // 移動速度をリセット
    Move();
    Attack();
    Guard();
    Jump();
    DodgeRoll();
}


//攻撃中
void Player::Impl::UpdateAttack() {
	if (m_Owner->is_GROUND) m_Owner->m_Velocity_f = 0.0f; //速度をリセット
    if (parryFg) m_Owner->m_Velocity_f = speed * 2.0f;//パリィ中は前進する
    
	if (!m_weapon) { return; }
    switch (m_attackkind) {
    case NONE:
        m_Owner->m_State = NORMAL;
        break;
    case SWING:
        if (m_Anim.IsPlaying()) {
			// 通常攻撃アニメーション更新、絶対値参照
			m_Owner->m_Rotation = m_Owner->m_ForwardRotation + m_Anim.UpdateAbsolute();//現在の前向き方向にアニメーション分を加算して算出する
        }
        if (m_weapon->GetMaxAttack()) {
            m_Owner->m_State = NORMAL;
            m_weapon->SwingEnd();
			m_Owner->m_Rotation.x = 0;// 攻撃終了時にX回転リセット
			m_attackkind = NONE;
			parryFg = false;// ジャストガードフラグリセット
        }
        break;

    case SPINSLASH:
		m_Owner->m_Rotation = m_Owner->m_ForwardRotation + m_Anim.UpdateAbsolute();// 回転切りアニメーション更新、絶対値参照
        ++attackframe;
		m_Owner->m_Velocity_f = speed;//回転切り中は現在方向に移動し続ける
        if (attackframe >= maxattackframe) {
            m_Owner->m_State = NORMAL;
            m_weapon->AttackEnd();
			speed = 1.0f;
			inviFg = false;
			m_Owner->SetColor({ 1,1,1,1 });
			m_attackkind = NONE;
        }
	    break;

    case SPINSLASH_VT:
        // 縦回転切中は移動可能
		Move();
        m_Owner->m_Rotation = m_Owner->m_ForwardRotation + m_Anim.UpdateAbsolute();// 回転切りアニメーション更新、絶対値参照
        ++attackframe;
		//　攻撃時間終了、もしくは地面に着地したら攻撃終了
        if (attackframe >= maxattackframe || m_Owner->is_GROUND) {
            m_Owner->m_State = NORMAL;
            m_weapon->AttackEnd();
            m_attackkind = NONE;
            m_Owner->m_Rotation.x = 0;
        }
        break;
    }
    
}

//ダメージ中
void Player::Impl::UpdateDamage() {
    m_Owner->m_Velocity_f = speed * -1;
    Guard();
    Attack();
    if (framecount > 10) {
        m_Owner->m_State = NORMAL;
        m_Owner->m_Scale = { 1.0f, 1.0f, 1.0f };
    }
}

//回避中
void Player::Impl::UpdateDodge() {
    m_Owner->m_Velocity_f = speed * 2.0f;
    ++rollcount;
    if (rollcount % 3 == 0) {
        // 残像エフェクト再生
        EffectParams  param;
        param.pos = m_Owner->m_Position;
		param.rot = m_Owner->m_Rotation;
        param.scale = m_Owner->m_Scale;
        param.maxLife = 10;
		param.color = { 0,0,1,0.3f };
        EffectManager::GetInstance()->Play(EFFECT_PLAYER, param);
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

    if (fabs(m_Owner->m_Position.x - m_ta_pos.x) < m_Owner->radius * 2&&
        fabs(m_Owner->m_Position.z - m_ta_pos.z) < m_Owner->radius * 2) {

		//カウンター攻撃処理
        m_Owner->m_Velocity_f = 0;
        invicount = 0;
        m_Owner->m_State = ATTACK;
        attackcombo = 0;
        SwingAttack(10, 6);
        
    }
    else {
        LookAt(m_ta_pos);
    }
	// カウンター攻撃終了判定
    if (m_weapon->GetMaxAttack()) {
        m_weapon->SwingEnd();
        invicount = 0;
	    m_Owner->m_State = NORMAL;
		Sound::GetInstance()->Play(SOUND_SE_SWINGVERTICAL);
    }
}

// パリィ中
void Player::Impl::UpdateParry() 
{
    // パリィ終了
    if (m_weapon->GetMaxAttack()) {
        m_weapon->SwingEnd();
        m_Owner->m_State = NORMAL;
    }
    
}


// 共通Update処理
void Player::Impl::UpdateCommon() {
    // コンボリセットの処理
    if(attackcombo > 0) {
        ++attackframe;
        if (attackframe >= maxattackframe)
        {
            attackcombo = 0;
            attackframe = 0;
        }
	}
    if (m_Owner->m_State != DODGE) {
        if (rollcount < rollcooldown) ++rollcount;

        if (inviFg) {
            if (invicount < maxinvicount) ++invicount;
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

    // プレイヤーの向きを前方ベクトルに合わせる、攻撃中は除く
    if (m_Owner->m_State != ATTACK)
        m_Owner->m_Rotation.y = m_Owner->m_ForwardRotation.y;

    m_Owner->BallUpdate();

    if (m_weapon)
        m_weapon->Update(m_Owner->m_Position, m_Owner->radius, m_Owner->m_Rotation);

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

        if (m_weapon && m_Owner->m_State == NORMAL)
        {
            m_weapon->Swing();
            m_Owner->m_State = ATTACK;
			m_attackkind = SWING;
        }
    }
}

