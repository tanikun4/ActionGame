#include <SimpleMath.h>

// イージング関数、加速度と現在フレームから求める
inline float EaseInOut(float t, float accel)
{
    // accel を 0?2 のカーブ強度に変換
    float k = 1.0f + accel * 2.0f;

    // EaseInOut の式（シグモイド的）
    float t2 = powf(t, k);
    float inv = powf(1.0f - t, k);

    return t2 / (t2 + inv);
}

// 角度アニメーション管理構造体
struct AngleAnim
{
    DirectX::SimpleMath::Vector3 start;
    DirectX::SimpleMath::Vector3 end;
    DirectX::SimpleMath::Vector3 current;
    int frame = 0;
    int maxFrame = 1;
    bool playing = false;
    float accel = 0.0f; // 加速度(0:等速,1:最大限かかる)


    // アニメーション開始
    void Start(const DirectX::SimpleMath::Vector3& s,
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)     // 加速度を追加
    {
        start = s;
        end = e;
        maxFrame = (f <= 0) ? 1 : f;
        frame = 0;
        playing = true;
        current = start;
        accel = (acc < 0.0f) ? 0.0f : (acc > 1.0f ? 1.0f : acc);
    }

    // 1フレーム更新して、現在の角度差を返す
    DirectX::SimpleMath::Vector3 Update()
    {
        if (!playing) return current;

        float t = (float)frame / (float)maxFrame;

        float eased_t = EaseInOut(t, accel);//加速度適用

        current = start + (end - start) * eased_t;

        if (frame >= maxFrame) {
            playing = false;
            current = end; // 最終値に
        }

        ++frame;
        return current;
    }

    bool IsPlaying() const { return playing; }
};
#pragma once
