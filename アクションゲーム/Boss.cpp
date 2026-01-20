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

bool Boss::GetLive() { return impl->GetLive(); }

void Boss::Damage(int _atk) { impl->Damage(_atk); }

void Boss::Stun(optional<Vector3> knockbackDir) { impl->Stun(knockbackDir); }

void Boss::ShotBullet() { impl->ShotBullet(); }

void Boss::SetDEF(int _def) { impl->SetDEF(_def); }

int Boss::GetHP() { return impl->GetHP(); }

Pole* Boss::GetWeapon() { return impl->GetWeapon(); }

void Boss::OnHit(Pole* po) { impl->OnHit(po); }

void Boss::OnHit(Bullet* bu) { impl->OnHit(bu); }

void Boss::OnHit(Projectile* pr) { impl->OnHit(pr); }

void Boss::OnHit(TestCube* cube) { impl->OnHit(cube); }