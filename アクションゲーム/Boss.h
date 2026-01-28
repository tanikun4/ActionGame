#pragma once
#include "Golfball.h"
#include <vector>
class Projectile;
class Pole;
class Bullet;

class Boss :
    public GolfBall
{
private:
	//pimplパターン、実装を隠す
	//Implクラスの宣言
	class Impl;
	std::unique_ptr<Impl> impl;
public:
	Boss(Camera * cam); // コンストラクタ
	~Boss();//デストラクタ

	void Init();
	void Update();
	void Draw();
	void Uninit();
	void Damage(int _atk);

	void SetGauge();//ゲージ初期化用、ゲームシーンでのみ呼び出す
	void SetTarget(class Player* player);
	void SetDEF(int _def);
	void ShotBullet();
	void Stun(std::optional<DirectX::SimpleMath::Vector3> knockbackDir = std::nullopt); // 行動不能状態にする
	bool GetLive();
	int GetHP();
	Pole* GetWeapon();
	void HitObject(Object* ob) override {
		ob->OnHit(this);
	}
	void OnHit(Pole* pole);
	void OnHit(Bullet* bu);
	void OnHit(TestCube* cube);
	void OnHit(Projectile* pr);
};

