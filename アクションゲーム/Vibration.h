#pragma once
#include <SimpleMath.h>
#include <cmath>

class Vibration
{
public:

    // amplitude : 振幅
    // speed     : 1フレームあたりの角速度
    // duration  : 持続フレーム（-1で無限）

    inline void Start(
        float amplitude,
        float frequency,
        float duration = -1.0f
    )
    {
        m_Amplitude = amplitude;
        m_Frequency = frequency;
        m_Duration = duration;
        m_Frame = 0.0f;
        m_Active = true;
    }

    inline void Stop()
    {
        m_Active = false;
    }

    inline bool IsActive() const
    {
        return m_Active;
    }

    inline DirectX::SimpleMath::Vector3 Update()
    {
        if (!m_Active)
            return DirectX::SimpleMath::Vector3::Zero;

        if (m_Duration > 0.0f && m_Frame >= m_Duration)
        {
            Stop();
            return DirectX::SimpleMath::Vector3::Zero;
        }

        float sx = sinf(m_Frame * m_Frequency);
        float sy = sinf(m_Frame * m_Frequency + 2.0f);
        float sz = sinf(m_Frame * m_Frequency + 4.0f);

        DirectX::SimpleMath::Vector3 offset(
            sx * m_Amplitude,
            sy * m_Amplitude,
            sz * m_Amplitude
        );

        ++m_Frame;

        return offset;
    }

    inline DirectX::SimpleMath::Vector3 UpdateMoveDir(
        const DirectX::SimpleMath::Vector3& moveDir
    )
    {
        using Vector3 = DirectX::SimpleMath::Vector3;

        // 振動していなければ何もしない
        if (!m_Active)
            return Vector3::Zero;

        // 通常の振動オフセット（既存ロジック）
        Vector3 rawOffset = Update();

        // 移動方向が無い場合はそのまま返す
        if (moveDir.LengthSquared() <= 0.0f)
            return rawOffset;

        Vector3 dir = moveDir;
        dir.Normalize();

        // 移動方向に直交するベクトル（XZ平面）
        Vector3 right(
            dir.z,
            0.0f,
            -dir.x
        );

        if (right.LengthSquared() <= 0.0f)
            return Vector3::Zero;

        right.Normalize();

        // 振動量を「直交方向のみに投影」
        float strength =
            rawOffset.x * right.x +
            rawOffset.y * right.y +
            rawOffset.z * right.z;

        return right * strength;
    }


private:
    DirectX::SimpleMath::Vector3 m_BasePos;

    float m_Frame = 0.0f;
    float m_Amplitude = 0.0f;
    float m_Frequency = 0.0f;
    float m_Duration = -1.0f;

    bool m_Active = false;
};
