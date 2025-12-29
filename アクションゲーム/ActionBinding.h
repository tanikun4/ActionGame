#pragma once
#include "Action.h"
#include <cstdint>

struct ActionBinding {
    int  key;   // VK_*
    uint16_t btn;   // XINPUT_GAMEPAD_*
};

