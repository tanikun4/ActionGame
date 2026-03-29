#pragma once
#include <Xinput.h>
#include <SimpleMath.h>
#pragma comment (lib, "xinput.lib") //XInputを使うために必要
class GamePadState {
public:
    void Update() {
        prev = curr;
        XInputGetState(0, &curr);
    }

    inline bool Press(WORD btn) const {
        return (curr.Gamepad.wButtons & btn) != 0;
    }

    // スティック取得関数
    DirectX::SimpleMath::Vector2 GetLeftStick() const {
        float x = curr.Gamepad.sThumbLX / 32767.0f;
        float y = curr.Gamepad.sThumbLY / 32767.0f;

        //現在の座標系と合わせるため逆転
        x = -x;
		y = -y;

        return ApplyDeadZone({ x, y });
    }

    DirectX::SimpleMath::Vector2 GetRightStick() const {
        float x = curr.Gamepad.sThumbRX / 32767.0f;
        float y = curr.Gamepad.sThumbRY / 32767.0f;

        //現在の座標系と合わせるため逆転
        x = -x;
        y = -y;

        return ApplyDeadZone({ x, y });
    }

private:
    DirectX::SimpleMath::Vector2 ApplyDeadZone(DirectX::SimpleMath::Vector2 v) const {
        float len = v.Length();
        const float dz = 0.2f;
        if (len < dz) return { 0,0 };

        // スムーズ補正
        float scale = (len - dz) / (1.0f - dz);
        v.Normalize();
        return v * scale;
    }

    XINPUT_STATE curr{};
    XINPUT_STATE prev{};
};

