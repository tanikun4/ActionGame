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
	void ReInit();
	void Damage(int _atk);

	void SetGauge();//ゲージ初期化用、ゲームシーンでのみ呼び出す
	void SetTarget(class Player* player);
	void SetDEF(int _def);
	void ShotBullet();
	void Stun();
	void SetNotUpdate(bool _notupdate);
	Pole* GetWeapon();
	int GetHP();
	void HitObject(Object* ob) override {
		ob->OnHit(this);
	}
	void OnHit(Pole* pole);
	void OnHit(Bullet* bu);
	void OnHit(TestCube* cube);
	void OnHit(Projectile* pr);
};

