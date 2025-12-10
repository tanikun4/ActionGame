#include <SimpleMath.h>

// 角度アニメーション管理構造体
struct AngleAnim
{
    DirectX::SimpleMath::Vector3 start;
    DirectX::SimpleMath::Vector3 end;
    DirectX::SimpleMath::Vector3 current;
    int frame = 0;
    int maxFrame = 1;
    bool playing = false;

    // アニメ開始
    void Start(const DirectX::SimpleMath::Vector3& s, const DirectX::SimpleMath::Vector3& e, int f)
    {
        start = s;
        end = e;
        maxFrame = (f <= 0) ? 1 : f;
        frame = 0;
        playing = true;
        current = start;
    }

    // 1フレーム更新して、現在の角度差を返す
    DirectX::SimpleMath::Vector3 Update()
    {
        if (!playing) return current;

        float t = (float)frame / (float)maxFrame;
        current = start + (end - start) * t;

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
