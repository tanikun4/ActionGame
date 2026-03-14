#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Object.h"
#include "ICollider.h"

//-----------------------------------------------------------------------------
//Cubeクラス
//-----------------------------------------------------------------------------
class Cube : 
	public Object , public ICollider
{ 
protected:
	float m_fLength[3] = {10.0f,10.0f,10.0f};             // 各軸方向の長さ

	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer	 m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ

	std::vector<VERTEX_3D> m_Vertices;//頂点データ

	// 描画の為の情報（見た目に関わる部分）
	Texture m_Texture; // テクスチャ
	const DirectX::SimpleMath::Vector3 m_length = { 10.0f,10.0f,10.0f }; // 各軸方向の長さの基準値

	bool m_Invisible = false;//透明かどうか

public:
	Cube(Camera* cam); // コンストラクタ
	Cube(); // コンストラクタ
	~Cube();//デストラクタ
	float GetLen(int elem) const;							  // 指定軸方向の長さを取得

	void InitCube();
	void Init();
	void DrawCube();
	void Draw();
	void UpdateCube();
	void Update();
	void Uninit();
	void SetInvisible(bool invisible) { m_Invisible = invisible; }

	// 色を指定
	void SetColor(const DirectX::SimpleMath::Vector4& color);

	void HitObject(Object* ob) override {
		ob->OnHit(this);
	}
	void OnHit(Object* ob) {};

	ICollider* GetCollider() override { return this; }
	Collision::ColliderVariant GetCollision() const override;
	Collision::OBB GetOBB() const; // OBBを取得
	//=======================================
	//移動処理
	//=======================================
	template<typename T>
	void Move(T _x, T _y, T _z) { //テンプレートを使う場合は、ヘッダーに処理内容も記述する必要がある
		m_Position.x += _x;
		m_Position.y += _y;
		m_Position.z += _z;
		return;
	}
};