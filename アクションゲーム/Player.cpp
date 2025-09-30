#include "Player.h"
#include <memory>
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

using namespace std;
using namespace DirectX::SimpleMath;
extern Input input;

// コンストラクタ
Player::Player(Camera* cam) : GolfBall(cam)
{
	m_arrow = nullptr;
}

// デストラクタ
Player::~Player()
{

}

void Player::DebugWeaponOffset() {//武器の位置を操作する
	ImGui::Begin("WeaponOffset");

	static Vector3 weapon_offset{};
	ImGui::SliderFloat3("WeaponOffset", &weapon_offset.x, -10.0f, 10.0f);

	if (ImGui::Button("Reset OffSet"))
			weapon_offset = Vector3(0, 0, 0);

	if (ImGui::Button("Set Int"))
			weapon_offset = Vector3((int)weapon_offset.x, (int)weapon_offset.y, (int)weapon_offset.z);

	if(m_pole)
		m_pole->SetOffsetDebug(weapon_offset);

	ImGui::End();
}

void Player::DebugPlayerStatus() {//プレイヤーの状態を操作する
	ImGui::Begin("PlayerStatus");

	ImGui::SliderFloat("radius", &radius, 0.0f, 10.0f);

	if (ImGui::Button("Reset radius"))
		radius = 4;

	static int select = 0;
	ImGui::RadioButton("Invisible", &select, 1);
	ImGui::RadioButton("Not_Invisible", &select, 0);
	
	if(select == 1) {
		inviFg = true;
		SetColor({ 0, 0, 1, 0.5f });
	}

	ImGui::End();
}

void Player::Init() {
	GBInit(u8"assets/model/gorufu/GolfBall_v2.fbx");
	m_Position = Vector3(0.0f, 50.0f, 0.0f);
	m_pole = Game::GetInstance()->AddObject<Pole>();
	m_pole->SetPl(true);
	hp = 9;
	flamecount = 30;

	// デバッグ関数の登録
	DebugUI::RedistDebugFunction([this]() {
		DebugWeaponOffset();
		});

	DebugUI::RedistDebugFunction([this]() {
		DebugPlayerStatus();
		});
}

void Player::Update() {
	m_Velocity_f = 0.0f;//はじめに移動速度を0にする
	//状態ごとの処理
	switch (m_State) {//0:通常時、1:近接攻撃中,2:遠距離攻撃中,3:ダメージ中,4:回避状態5:カウンター状態
	case 0:
		UpdateNormal();
		/*Move();
		Attack();
		Guard();
		if (Input::GetKeyPress(VK_CONTROL)) {
			Counter();
		}*/
		break;
	case 1:
		UpdateAttack();
		/*if (m_pole->GetState() == 0) {
			m_State = 0;
		}*/
		break;
	case 2:
		if (Input::GetKeyPress(VK_A))
		{
			m_Rotation.y -= 0.1;

		}
		else if (Input::GetKeyPress(VK_D))
		{
			m_Rotation.y += 0.1;
		}

		Charge();

		if (Input::GetKeyRelease(VK_SHIFT)) {
			Shot();
			m_State = 0;
			Sound::GetInstance()->Play(SOUND_SE_ARROWSHOT);
		}
		break;
	case 3:
		UpdateDamage();
		/*m_Velocity_f = speed * -1;
		Guard();
		if (flamecount > 10) {
			m_State = 0;
		}*/
		break;
	case 4:
		UpdateDodge();
		/*m_Position += m_ForwardVector * speed * 2.0f;
		rollcount++;
		if (rollcount > 10) {
			RollFg = false;
			m_State = 0;
			inviFg = false;
			rollcount = 0;
			SetColor({ 1, 1, 1, 1 });
		}*/
		break;
	case 5:
		UpdateCounter();
		/*if (fabs(m_Position.x - m_ta_pos.x) < radius * 6 && fabs(m_Position.z - m_ta_pos.z) < radius * 6) {
			m_pole->Swing();
			m_State = 1;
			inviFg = false;
			SetColor({ 1, 1, 1, 1 });
		}
		else {
			Boss* boss = Game::GetInstance()->GetObjects<Boss>()[0];
			boss->GetPosition();
			LookAt(boss->GetPosition());
			m_Velocity_f = speed * 3;
		}*/
		break;
	}

	if (m_State != 4) {
		if (rollcount < rollcooldown) {
			++rollcount;
		}
		if (inviFg) {
			if (invicount < 60) {
				++invicount;
			}
			else {
				inviFg = false;
				SetColor({ 1, 1, 1, 1 });
				invicount = 0;
			}
		}
		if (flamecount < 60) {
			++flamecount;
		}
		if (GuardFg) {
			++guardcount;
		}
	}

	else {
		//CheckHit();
	}
	// 下に落ちた時はダメージを受けてリスポーン
	if (m_Position.y < -100)
	{
		hp -= 1;
		m_Position = Vector3(0.0f, 50.0f, 0.0f);
		m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	}
	GBUpdate();
	m_pole->Update(m_Position, radius, m_Rotation,1.7f );
}



void Player::Move() {
	//キー入力による移動
	if (Input::GetKeyPress(VK_W))
	{
		// 前進
		if (Input::GetKeyPress(VK_A)) {
			m_Rotation.y = 3 * PI / 4;
		}
		else if (Input::GetKeyPress(VK_D)) {
			m_Rotation.y = 5 * PI / 4;
		}
		else {
			m_Rotation.y = PI;
		}
		m_Velocity_f = speed;
	}
	else if (Input::GetKeyPress(VK_S))
	{
		// 後退
		if (Input::GetKeyPress(VK_A)) {
			m_Rotation.y = PI / 4;
		}
		else if (Input::GetKeyPress(VK_D)) {
			m_Rotation.y = 7 * PI / 4;
		}
		else {
			m_Rotation.y = 0.0f;
		}
		m_Velocity_f = speed;
	}
	else if (Input::GetKeyPress(VK_A))
	{
		//左移動
		m_Rotation.y = PI / 2;
		m_Velocity_f = speed;

	}
	else if (Input::GetKeyPress(VK_D))
	{
		//右移動
		m_Rotation.y = 3 * PI / 2;
		m_Velocity_f = speed;

	}
	if (Input::GetKeyTrigger(VK_J)) {
		if (rollcount >= rollcooldown) {
			DodgeRoll();
		}
	}
}

void Player::DodgeRoll() {
	m_State = 4;
	rollcount = 0;
	inviFg = true;
	RollFg = true;
	SetColor({0, 0, 1, 0.5});
}

void Player::Attack() {
	if (Input::GetKeyTrigger(VK_K) && !GuardFg) { //ガード状態でなければ攻撃する
		m_pole->Swing();
		m_State = 1;
		Sound::GetInstance()->Play(SOUND_SE_SWING);
	}
	/*if (Input::GetKeyTrigger(VK_SHIFT)) {
		Charge();
		m_State = 2;
		Sound::GetInstance()->Play(SOUND_SE_ARROWCHARGE);
	}*/
}

void Player::Charge() {
	if (!m_arrow) {
		vector<Bullet*> bullet = Game::GetInstance()->GetObjects<Bullet>();
		for (auto& bu : bullet) {
			if (bu->GetState() == 0) {
				m_arrow = bu;
				break;
			}
		}
		if (!m_arrow) {
			return;
		}
	}
	m_arrow->PCharge(m_Rotation.y,radius,m_Position);
}

void Player::Shot() {
	if (!m_arrow) {
		vector<Bullet*> bullet = Game::GetInstance()->GetObjects<Bullet>();
		for (auto& bu : bullet) {
			if (bu->GetState() == 0) {
				m_arrow = bu;
				break;
			}
		}
		if (!m_arrow) {
			return;
		}
	}
	m_arrow->PShot(m_Rotation.y, radius, m_Position);
	m_arrow = nullptr;
}

void Player::CheckHit() { // 以前に使っていた当たり判定(もう使わない)
	Collision::Sphere balCollision = { m_Position, radius };
	/*vector<Enemy*> enemy = Game::GetInstance()->GetObjects<Enemy>();
	for (auto& en : enemy) {
		if (en->GetLive()) {
			Collision::Sphere enbalCollision = { en->GetPosition(), en->GetRadius() };
			if (Collision::CheckHit(enbalCollision, balCollision)) {
				Damage(1);
				Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
				return;
			}
		}
	}*/

	Boss* boss = Game::GetInstance()->GetObjects<Boss>()[0];
	Collision::Sphere bossbalCollision = { boss->GetPosition(), boss->GetRadius() };
	if (Collision::CheckHit(bossbalCollision, balCollision)) {
		Damage(1);
		Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
		return;
	}

	vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
	for (auto& ar : arrow) {
		if (ar->GetId() == 2) {
			if (Collision::CheckHit(ar->hitbox, balCollision)) {
				Damage(ar->GetAtk());
				Sound::GetInstance()->Play(SOUND_SE_ARROWHIT);
				return;
			}
		}
	}
	return;
}

void Player::CheckHitPole(Pole* pole) { // 以前に使っていたPoleの当たり判定(もう使わない)
	if (m_State == 3) { return; }
	Collision::Sphere balCollision = { m_Position , radius };
	if (pole->GetState() == 1) {
		if (Collision::CheckHit(pole->hitbox, balCollision)) {
			Damage(2);
			Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
		}
	}
	return;
}

void Player::Damage(int atk) { //ダメージ時の処理
	if (inviFg == false) {
		if(GuardFg){
			if (atk > 1) 
				atk = atk / 2;
		}
		hp -= atk;
		m_State = 3;
		flamecount = 0;
		inviFg = true;
		SetColor(Vector4(1, 1, 0, 0.5));
		if(GuardFg)
			Sound::GetInstance()->Play(SOUND_SE_PLAYERGUARD);
		else
			Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
	}
}

int Player::GetHP() {
	return hp;
}

Pole* Player::GetWeapon() {
	return m_pole;
}

void Player::Guard() {
	if (Input::GetKeyTrigger(VK_I)) {
		GuardFg = true;
		guardcount = 0;
		speed = 0.1;
		m_pole->GuardStart();
	}
	if (Input::GetKeyRelease(VK_I)) {
		GuardFg = false;
		speed = 1;
		m_pole->GuardEnd();
	}
	return;
}

void Player::Counter()
{
	Boss* boss = Game::GetInstance()->GetObjects<Boss>()[0];
	boss->GetPosition();
	LookAt(boss->GetPosition());
	rollcount = 0;
	m_State = 5;
	speed = 1.0f;
	RollFg = false;
	GuardFg = false;
	inviFg = true;
	SetColor({ 0, 0, 1, 0.5 });
	Sound::GetInstance()->Play(SOUND_SE_PLAYERJUSTGUARD);
}

void Player::LookAt(Vector3 ta_pos) {
	// atan2を使用して角度を求める
	m_Rotation.y = atan2f((ta_pos.x - m_Position.x), (ta_pos.z - m_Position.z));
	m_ta_pos = ta_pos;
}

void Player::OnHit(Boss* bo) {
	Damage(1);
	return;
}

void Player::OnHit(Pole* po) {
	const int damage = 2;
	if (po->GetPl()) return;
	if (RollFg && rollcount < 5 && po->GetSwingTime() < 5) { Counter(); return; };
	if (GuardFg && guardcount < 60 && po->GetSwingTime() < 10) { Counter(); return; }
	Damage(damage);

}

void Player::OnHit(Bullet* bu) {
	if (bu->GetPl()) return;
	if (GuardFg && guardcount < 60) { Counter(); return; }
	const int damage = 1;
	Damage(damage);
}

void Player::UpdateNormal() {
	Move();
	Attack();
	Guard();
	if (Input::GetKeyPress(VK_CONTROL)) {//テスト用、カウンター攻撃を発動する
		Counter();
	}
}

void Player::UpdateAttack() {
	if (m_pole->GetState() == 0) {
		m_State = 0;
	}
}

void Player::UpdateDamage() {
	// ノックバックする
	m_Velocity_f = speed * -1;
	Guard();
	if (flamecount > 10) {
		m_State = 0;
	}
}

void Player::UpdateDodge() {
	//回避中
	m_Position += m_ForwardVector * speed * 2.0f;
	rollcount++;
	if (rollcount > 10) {
		RollFg = false;
		m_State = 0;
		inviFg = false;
		rollcount = 0;
		SetColor({ 1, 1, 1, 1 });
	}
}

void Player::UpdateCounter() {
	//カウンター攻撃中
	if (fabs(m_Position.x - m_ta_pos.x) < radius * 6 && fabs(m_Position.z - m_ta_pos.z) < radius * 6) {
		m_pole->Swing();
		m_State = 1;
		inviFg = false;
		SetColor({ 1, 1, 1, 1 });
	}
	else {
		Boss* boss = Game::GetInstance()->GetObjects<Boss>()[0];
		boss->GetPosition();
		LookAt(boss->GetPosition());
		m_Velocity_f = speed * 3;
	}
}

