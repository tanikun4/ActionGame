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
	DirectX::SimpleMath::Vector3 prev;
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
		prev = start;
        accel = std::clamp(acc, 0.0f, 1.0f);
    }

    DirectX::SimpleMath::Vector3 Update()
    {
        if (!playing) return current;

        float t = (float)frame / (float)maxFrame;
        float eased_t = EaseInOut(t, accel);
		prev = current;
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
        return position + Update() - prev;
    }

    // 相対座標用、処理自体は絶対座標と同じ
    DirectX::SimpleMath::Vector3 UpdateRelative()
    {
        return Update() - prev;
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
    DirectX::SimpleMath::Vector3 prev;
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
        prev = start;          
        current = start;
        accel = std::clamp(acc, 0.0f, 1.0f);
    }

    // 1フレーム更新して、現在の角度を返す
    DirectX::SimpleMath::Vector3 Update()
    {
        if (!playing) return current;

        float t = (float)frame / (float)maxFrame;

        float eased_t = EaseInOut(t, accel);//加速度適用

        prev = current;
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
        return rotation + Update() - prev;
    }

	// 相対角度用、前回角度との差分を返す
    DirectX::SimpleMath::Vector3 UpdateRelative()
    {
        return Update() - prev;
    }

    bool IsPlaying() const { return playing; }

	// リセット関数
    void Reset() {
        playing = false;
		current = { 0,0,0 };
    }
};

enum class ArcRotateDir
{
    CW,   // 時計回り
    CCW   // 反時計回り
};

enum class ArcMoveDir
{
    Forward,   // 前進
    Backward   // 後退
};

struct ArcMoveAnim
{
private:
    AngleAnim angleAnim;   // xyz角度（ラジアン）
    PositionAnim radiusAnim;
    PositionAnim heightAnim;

    DirectX::SimpleMath::Vector3 center;

    DirectX::SimpleMath::Vector3 prevPos; // 前フレームの絶対位置
    bool first = true;

    ArcRotateDir rotateDir = ArcRotateDir::CCW;
    ArcMoveDir   moveDir = ArcMoveDir::Forward;

public:
    // 開始
    void Start(
        const float& startAngle,
        const float& endAngle,
        float radius,
        int frame,
        float angleAccel = 0.0f,
        ArcRotateDir rDir = ArcRotateDir::CCW,
        ArcMoveDir mDir = ArcMoveDir::Forward,
        float heightStart = 0.0f,
        float heightEnd = 0.0f,
        float heightAccel = 0.0f)
    {
        first = true;
        rotateDir = rDir;
        moveDir = mDir;

        // xyz角度をそのまま補間
        angleAnim.StartAbsolute(
            { 0, startAngle, 0 },
            { 0, endAngle, 0 },
            frame,
            angleAccel
        );

        radiusAnim.StartAbsolute(
            { radius,0,0 },
            { radius,0,0 },
            frame
        );

        heightAnim.StartAbsolute(
            { heightStart,0,0 },
            { heightEnd,0,0 },
            frame,
            heightAccel
        );
    }

    // 前フレームからの変化量を返す
    DirectX::SimpleMath::Vector3 Update()
    {
        //auto angle = angleAnim.UpdateAbsolute();
        float radius = radiusAnim.UpdateAbsolute().x;
        float height = heightAnim.UpdateAbsolute().x;

        // 回転方向
        int rotSign = (rotateDir == ArcRotateDir::CCW) ? 1 : -1;

        float yaw = angleAnim.UpdateAbsolute().y * rotSign;

        // 今フレームの位置を算出（XZ円弧 + Y）
        DirectX::SimpleMath::Vector3 currentPos;
        currentPos.x = std::sinf(yaw) * radius;
        currentPos.z = std::cosf(yaw) * radius;
        currentPos.y = center.y + height;

        if (first)
        {
            prevPos = currentPos;
            first = false;
            return { 0,0,0 };
        }

        DirectX::SimpleMath::Vector3 delta = currentPos - prevPos;
        prevPos = currentPos;
        // 進行方向
       int moveSign = (moveDir == ArcMoveDir::Forward) ? -1 : 1;
        return delta * (float)moveSign;
    }

    bool IsPlaying() const
    {
        return angleAnim.IsPlaying();
    }

    void Reset()
    {
        angleAnim.Reset();
        radiusAnim.Reset();
        heightAnim.Reset();
        first = true;
    }
};

// 黄金螺旋移動構造体
struct FibonacciAnim
{
private:
    AngleAnim angleAnim; // y = θ
    DirectX::SimpleMath::Vector3 center;

    DirectX::SimpleMath::Vector3 prevPos;
    bool first = true;

    float baseRadius = 1.0f;
    const float phi = 1.61803398875f;

public:
    void Start(
        float startAngle,
        float endAngle,
        float radius,
        int frame,
        float angleAccel = 0.0f)
    {
        baseRadius = radius;
        first = true;

        angleAnim.StartAbsolute(
            { 0, startAngle, 0 },
            { 0, endAngle, 0 },
            frame,
            angleAccel
        );
    }

    DirectX::SimpleMath::Vector3 Update()
    {
        auto angle = angleAnim.UpdateAbsolute();
        float theta = angle.y;

        // 黄金螺旋：90度で φ 倍
        float r = baseRadius * powf(phi, theta / (DirectX::XM_PI / 2.0f));

        DirectX::SimpleMath::Vector3 current;
        current.x = sinf(theta) * r;
        current.z = cosf(theta) * r;
        //current.y = center.y;

        if (first)
        {
            prevPos = current;
            first = false;
            return { 0,0,0 };
        }

        auto delta = current - prevPos;
        prevPos = current;
        return delta;
    }

    bool IsPlaying() const
    {
        return angleAnim.IsPlaying();
    }

    void Reset()
    {
        angleAnim.Reset();
        first = true;
    }
};

