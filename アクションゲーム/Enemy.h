#pragma once
#include "GolfBall.h"
class Arrow;

class Enemy :
    public GolfBall
{
private:
	int hp = 10;
	int flamecount = 0;
	int m_State = 0;//状態　1で行動中
	DirectX::XMFLOAT2 stagesize;
	DirectX::SimpleMath::Vector3 hitbackrotation = { 0,0,0 };//攻撃に当たった時のノックバックする向き
	Arrow* m_arrow;

	void Attack();
	void SetArrow();
	bool HitCheck();//攻撃に当たったかを返す
public:
	Enemy(Camera* cam); // コンストラクタ
	~Enemy();//デストラクタ

	void Init();
	void Update();
	void Draw();
	void Damage(int atk);
	void SetStageSize(DirectX::XMFLOAT2 _stagesize);
	bool GetLive();//生存しているかを返す
};

