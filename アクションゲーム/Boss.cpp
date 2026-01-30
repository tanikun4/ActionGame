#include "Boss.h"
#include "BossImpl.h"

using namespace std;
using namespace DirectX::SimpleMath;

Boss::Boss(Camera* cam)
	: GolfBall(cam)
	, impl(std::make_unique<Impl>(cam, this))
{

}


Boss::~Boss() = default;

void Boss::Init() { impl->Init(); }

void Boss::Update() { impl->Update(); }

void Boss::Draw() { impl->Draw(); }

void Boss::Uninit() { impl->Uninit(); }

void Boss::ReInit() { impl->ReInit(); }

void Boss::Damage(int _atk) { impl->Damage(_atk); }

void Boss::Stun() { impl->Stun(); }

void Boss::ShotBullet() { impl->ShotBullet(); }

void Boss::SetGauge() { impl->SetGauge(); }

void Boss::SetTarget(Player* player) { impl->SetTarget(player); }

void Boss::SetDEF(int _def) { impl->SetDEF(_def); }

void Boss::SetNotUpdate(bool _notupdate) { impl->SetNotUpdate(_notupdate); }

int Boss::GetHP() { return impl->GetHP(); }

Pole* Boss::GetWeapon() { return impl->GetWeapon(); }

void Boss::OnHit(Pole* po) { impl->OnHit(po); }

void Boss::OnHit(Bullet* bu) { impl->OnHit(bu); }

void Boss::OnHit(Projectile* pr) { impl->OnHit(pr); }

void Boss::OnHit(TestCube* cube) { impl->OnHit(cube); }