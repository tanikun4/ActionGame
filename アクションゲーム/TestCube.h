#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

//-----------------------------------------------------------------------------
//TestCubeクラス
//-----------------------------------------------------------------------------
class TestCube {
protected:
	DirectX::SimpleMath::Vector3 m_NormaDirect[3] = { {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f} };   // 方向ベクトル
	float m_fLength[3] = {10.0f,10.0f,10.0f};             // 各軸方向の長さ

	// SRT情報（姿勢情報）
	DirectX::SimpleMath::Vector3 m_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer	 m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ

	// 描画の為の情報（見た目に関わる部分）
	Shader m_Shader; // シェーダー
	Texture m_Texture; // テクスチャ
	DirectX::SimpleMath::Vector3 m_length = {20.0f,20.0f,20.0f};

public:
	DirectX::SimpleMath::Vector3 GetPos() const;
	DirectX::SimpleMath::Vector3 GetDirect(int elem) const;   // 指定軸番号の方向ベクトルを取得
	float GetLen(int elem) const;							// 指定軸方向の長さを取得

	void SetPos(DirectX::SimpleMath::Vector3 v);
	void SetRotation(DirectX::SimpleMath::Vector3 v);
	void SetScale(DirectX::SimpleMath::Vector3 v);
	void SetMaxMinVertices();
	void InitCube();
	void Init();
	void DrawCube();
	void Draw();
	void UpdateCube();
	void Update();
	void Uninit();
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