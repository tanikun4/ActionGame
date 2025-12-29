#pragma once
#include <Windows.h>

class KeyboardState {
public:
    void Update() {
        memcpy(prev, curr, sizeof(curr));
        GetKeyboardState(curr);
    }

    inline bool Press(int key) const {
        return (curr[key] & 0x80) != 0;
    }

private:
    BYTE curr[256]{};
    BYTE prev[256]{};
};
