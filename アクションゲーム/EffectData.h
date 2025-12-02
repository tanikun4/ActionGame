#pragma once
#include <string>
#include <SimpleMath.h>
//エフェクトに渡すデータの構造体定義
struct EffectData
{
	std::string model = ""; // モデル名
	std::string texture = ""; // テクスチャ名
	int maxlife = 60; // 最大生存フレーム
	DirectX::SimpleMath::Vector3 _ta_scale = { -1, -1, -1 };//最終スケール
};