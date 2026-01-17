#pragma once
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

// 座標アニメーション構造体
struct PositionAnim
{
private:
    DirectX::SimpleMath::Vector3 start;
    DirectX::SimpleMath::Vector3 end;
    DirectX::SimpleMath::Vector3 current;
    int frame = 0;
    int maxFrame = 1;
    bool playing = false;
    float accel = 0.0f;

    void Start(
        const DirectX::SimpleMath::Vector3& s,
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        start = s;
        end = e;
        maxFrame = (f <= 0) ? 1 : f;
        frame = 0;
        playing = true;
        current = start;
        accel = std::clamp(acc, 0.0f, 1.0f);
    }

    DirectX::SimpleMath::Vector3 Update()
    {
        if (!playing) return current;

        float t = (float)frame / (float)maxFrame;
        float eased_t = EaseInOut(t, accel);

        current = start + (end - start) * eased_t;

        if (frame >= maxFrame) {
            playing = false;
            current = end;
        }

        ++frame;
        return current;
    }

public:
    // 絶対座標
    void StartAbsolute(
        const DirectX::SimpleMath::Vector3& s,
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        Start(s, e, f, acc);
    }

    // 現在座標から相対移動、開始点ありで指定する
    void StartRelative(
        const DirectX::SimpleMath::Vector3& s,
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        Start(s, e, f, acc);
    }


    // 相対座標の開始関数、こちらは開始点無しで指定する
    void StartRelative(
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        Start({ 0,0,0 }, e, f, acc);
    }

	// 絶対座標用
    DirectX::SimpleMath::Vector3 UpdateAbsolute()
    {
        return Update();
    }

    // 相対座標用、指定座標と足し合わせた値を返す
    DirectX::SimpleMath::Vector3 UpdateRelative(const DirectX::SimpleMath::Vector3& position)
    {
        return position + Update();
    }

    // 相対座標用、処理自体は絶対座標と同じ
    DirectX::SimpleMath::Vector3 UpdateRelative()
    {
        return Update();
    }

    bool IsPlaying() const { return playing; }

    // リセット関数
    void Reset() {
        playing = false;
        current = { 0,0,0 };
    }
};

// 角度アニメーション管理構造体
struct AngleAnim
{

private:
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
        accel = std::clamp(acc, 0.0f, 1.0f);
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
public:
	// 絶対角度の開始関数
    void StartAbsolute(const DirectX::SimpleMath::Vector3& s,
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        Start(s, e, f, acc);
	}

	// 相対角度の開始関数、開始点ありで指定する
    void StartRelative(
		const DirectX::SimpleMath::Vector3& s,
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        Start(s, e, f, acc);
    }


	// 相対角度の開始関数、こちらは開始点無しで指定する
    void StartRelative(
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        Start({ 0,0,0 }, e, f, acc);
	}

	// 現在角度からの相対角度開始関数
    void StartRelativeCurrent(
        const DirectX::SimpleMath::Vector3& e,
        int f,
        float acc = 0.0f)
    {
        Start(current, e, f, acc);
    }

    // 絶対角度用
    DirectX::SimpleMath::Vector3 UpdateAbsolute()
    {
        return Update();
    }

    // 相対角度用、指定角度と足し合わせた値を返す
    DirectX::SimpleMath::Vector3 UpdateRelative(const DirectX::SimpleMath::Vector3& rotation)
    {
        return rotation + Update();
    }

    // 相対角度用、処理自体は絶対角度と同じ
    DirectX::SimpleMath::Vector3 UpdateRelative()
    {
        return Update();
    }

    bool IsPlaying() const { return playing; }

	// リセット関数
    void Reset() {
        playing = false;
		current = { 0,0,0 };
    }
};
