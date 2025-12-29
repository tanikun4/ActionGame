#pragma once
#include <Xinput.h>

class GamePadState {
public:
    void Update() {
        prev = curr;
        XInputGetState(0, &curr);
    }

    inline bool Press(WORD btn) const {
        return (curr.Gamepad.wButtons & btn) != 0;
    }

private:
    XINPUT_STATE curr{};
    XINPUT_STATE prev{};
};

