#include "Enemy.h"
#include "EnemyImpl.h"

using namespace std;
using namespace DirectX::SimpleMath;

Enemy::Enemy(Camera* cam)
    : GolfBall(cam)
    , impl(std::make_unique<Impl>(cam, this))
{
}

Enemy::~Enemy() = default;

void Enemy::Init() { impl->Init(); }
void Enemy::Update() { impl->Update(); }
void Enemy::Draw() { impl->Draw(); }
void Enemy::Uninit() { impl->Uninit(); }

int Enemy::GetHP() { return impl->GetHP(); }
Pole* Enemy::GetWeapon() { return impl->GetWeapon(); }

void Enemy::OnHit(Pole* po) { impl->OnHit(po); }
void Enemy::OnHit(Bullet* bu) { impl->OnHit(bu); }
void Enemy::OnHit(Projectile* pr) { impl->OnHit(pr); }
void Enemy::OnHit(TestCube* cube) { impl->OnHit(cube); }