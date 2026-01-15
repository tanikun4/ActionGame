#pragma once
#include "Object.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "Material.h"
#include "input.h"
#include "Collision.h"
#include "ICollider.h"
#include "ShadowCircle.h"
#include "Vibration.h"

class GolfBall :public Object , public ICollider
{
protected:

	// 描画の為の情報（メッシュに関わる情報）
	MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

	// 描画の為の情報（見た目に関わる部分）
	std::vector<std::unique_ptr<Material>> m_Materiales;
	std::vector<SUBSET> m_subsets;
	std::vector<std::unique_ptr<Texture>> m_Textures; // テクスチャ

	int m_State = 0; //状態変数、クラスによって意味が異なる
	int m_StopCount = 0; // 静止カウント
	float radius = 4.0f;//ボールモデルの直径
	bool is_GROUND = false;//地面に接地しているかどうかのフラグ

	float m_Velocity_f = 0.0; // 現在の移動速度（前進後退）
	DirectX::SimpleMath::Vector3 m_oldPos;
	static constexpr float ForwardVelocity = 0.5f; // 前進速度定数
	static constexpr float BackwardVelocity = 0.5f; // 後退速度定数
	static constexpr float LeftRightVelocity = 0.5f; // 左右移動速度定数
	
	const float gravity = 0.07f; //重力

	DirectX::SimpleMath::Vector3 ToCameraEffectPos(DirectX::SimpleMath::Vector3 _pos,float _dist);//対象の位置からカメラ方向へ指定距離だけ向かう座標を返す(エフェクトの位置調整に使用)

	ShadowCircle* m_Shadow;

public:

	GolfBall();
	GolfBall(Camera* cam); // コンストラクタ
	~GolfBall();//デストラクタ

	void GBInit(std::u8string modelfilename);
	void GBUpdate();
	void GBDraw();
	void Init();
	void Update();
	void Draw();
	void Uninit();
	bool CheckGround();

	

	// 状態の設定・取得
	void SetState(int s);
	int GetState();

	// ショット
	void Shot(DirectX::SimpleMath::Vector3 v);

	float GetRadius();
	Collision::ColliderVariant GetCollision();
	DirectX::SimpleMath::Vector3 GetForwardVector();

	void SetColor(const DirectX::SimpleMath::Vector4& color);//色を変える
};

