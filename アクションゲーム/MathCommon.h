#pragma once
//数学関連共通ヘッダ
#define PI  (DirectX::XM_PI)
#include <SimpleMath.h>
struct Int2 {
	int x;
	int y;
};

// ラジアンを -PI ～ PI の範囲に正規化する
inline float NormalizeRad(float rad)
{
    const float TWO_PI = PI * 2.0f;

    while (rad > PI)  rad -= TWO_PI;
    while (rad < -PI) rad += TWO_PI;

    return rad;
}

// min ～ max内のランダム値を返す
inline float RandRange(float min, float max)
{
    return min + (max - min) * (rand() / (float)RAND_MAX);
}