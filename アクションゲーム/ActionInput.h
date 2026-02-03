#pragma once
#include "Action.h"
#include "ActionState.h"
#include "ActionBinding.h"
#include "KeyboardState.h"
#include "GamePadState.h"

class ActionInput {
public:
    static ActionInput& GetInstance();

    void Init();
    void Update();
    void Uninit();

    bool IsPress(Action a)   const;
    bool IsTrigger(Action a) const;
    bool IsRelease(Action a) const;

    float GetMoveDirectionRad() const;
private:
    ActionInput() = default;                  // äOïîê∂ê¨ã÷é~
    ~ActionInput() = default;
    ActionInput(const ActionInput&) = delete;
    ActionInput& operator=(const ActionInput&) = delete;
private:
    KeyboardState keyboard;
    GamePadState  gamepad;

    ActionState   states[(int)Action::ActionMax];
    ActionBinding bindings[(int)Action::ActionMax] = {};
};
