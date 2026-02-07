#pragma once
#include "MathCommon.h"
#include "Shader.h"
#include "Camera.h"

class Object  {
protected:
	// SRT情報（姿勢情報）
	DirectX::SimpleMath::Vector3 m_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

	DirectX::SimpleMath::Vector3 m_OldRotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);//前フレームの回転情報

	DirectX::SimpleMath::Vector3 m_ForwardRotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);//前向きの回転情報(進行方向)
	DirectX::SimpleMath::Vector3 m_OldForwardRotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);//前フレームの回転情報

	//速度
	DirectX::SimpleMath::Vector3 m_Velocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	//加速度
	DirectX::SimpleMath::Vector3 m_Acceralation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	//前向きベクトル変数
	DirectX::SimpleMath::Vector3 m_ForwardVector = { 0.0f,0.0f,1.0f };//初期ベクトル

	// 描画の為の情報（見た目に関わる部分）
	Shader m_Shader; // シェーダー

	// カメラ
	Camera* m_Camera;
	bool m_live = true;

	bool UpdateDirectionVectors();//行列更新関数
	bool UpdateForwardDirectionVectors();//行列更新関数(進行方向版)
	DirectX::SimpleMath::Vector3 AngleToForward(const DirectX::SimpleMath::Vector3& rot);//回転角から前向きベクトルを求める関数

public:

	Object();
	Object(Camera* cam); // コンストラクタ

	virtual ~Object(); // デストラクタ

	virtual void Init()=0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Uninit() = 0;
	// 衝突処理の入口
	virtual void HitObject(Object* ob) { ob->OnHit(this); };//オブジェクトに当たった時、相手側のHit処理を実行
	
	virtual void OnHit(Object* ob) { return; }//オブジェクトに当たった時の処理を入れる

	// 派生用
	virtual void OnHit(class Player* player) {} 
	virtual void OnHit(class Boss* boss) {}    
	virtual void OnHit(class Enemy* enemy) {}
	virtual void OnHit(class Sword* pole) {}    
	virtual void OnHit(class Bullet* bullet) {} 
	virtual void OnHit(class Projectile* projectile) {}
	virtual void OnHit(class BallObject* ball) {} 
	virtual void OnHit(class Cube* cube) {};

	virtual bool GetLive() { return m_live; }

	//position、rotation、scale セッター関数・ゲッター関数
	void SetPosition(const DirectX::SimpleMath::Vector3& pos);
	void SetRotation(const DirectX::SimpleMath::Vector3& rot);
	void SetScale(const DirectX::SimpleMath::Vector3& sca);
	void SetLive(const bool& live);
	void SetCamera(Camera* cam) { m_Camera = cam; };
	DirectX::SimpleMath::Vector3 GetPosition() const;
	DirectX::SimpleMath::Vector3 GetRotation() const;
	DirectX::SimpleMath::Vector3 GetScale() const;
	DirectX::SimpleMath::Vector3 GetForwardRotation() const;
	virtual class ICollider* GetCollider() { return nullptr; }

};
