#pragma once
#include <SimpleMath.h>
#include <cmath>

// 座標振動用クラス
class Vibration
{
public:

    // amplitude : 振幅
    // speed     : 1フレームあたりの角速度
    // duration  : 持続フレーム（-1で無限）

    inline void Start(
        float amplitude,
        float frequency,
        int duration = -1
    )
    {
        m_Amplitude = amplitude;
        m_Frequency = frequency;
        m_Duration = duration;
        m_Frame = 0;
        m_Active = true;
    }

	// 振動を停止する
    inline void Stop()
    {
        m_Active = false;
    }

	// 振動が有効か
    inline bool IsActive() const
    {
        return m_Active;
    }

	// 振動を更新し、現在のオフセットを返す
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
	// moveDir : 振動の移動方向
    inline DirectX::SimpleMath::Vector3 UpdateMoveDir(
        const DirectX::SimpleMath::Vector3& moveDir
    )
    {
        using Vector3 = DirectX::SimpleMath::Vector3;

        if (!m_Active)
            return Vector3::Zero;

        // 振動量
        Vector3 rawOffset = Update();

        if (moveDir.LengthSquared() <= 0.0f)
            return rawOffset;

        // XZ平面の正規化
        Vector3 dir = moveDir;
        dir.y = 0.0f;
        if (dir.LengthSquared() <= 0.0f)
            return Vector3::Zero;
        dir.Normalize();

        // 右方向ベクトル
        Vector3 right(-dir.z, 0.0f, dir.x);

        // 左右振動
        Vector3 offset = right * rawOffset.x;

        // Y方向の振動も残したい場合
        //offset.y = rawOffset.y;

        return offset;
    }

    


private:
    DirectX::SimpleMath::Vector3 m_BasePos;

    int m_Frame = 0;
    float m_Amplitude = 0.0f;
    float m_Frequency = 0.0f;
    int m_Duration = -1;

    bool m_Active = false;
};
