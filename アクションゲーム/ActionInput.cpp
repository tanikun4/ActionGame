#include "ActionInput.h"
#include "MathCommon.h"

ActionInput& ActionInput::GetInstance()
{
    static ActionInput instance;
    return instance;
}

void ActionInput::Init()
{
    bindings[(int)Action::MoveUp] = { 'W', XINPUT_GAMEPAD_DPAD_UP };
    bindings[(int)Action::MoveDown] = { 'S', XINPUT_GAMEPAD_DPAD_DOWN };
    bindings[(int)Action::MoveLeft] = { 'A', XINPUT_GAMEPAD_DPAD_LEFT };
    bindings[(int)Action::MoveRight] = { 'D', XINPUT_GAMEPAD_DPAD_RIGHT };
    bindings[(int)Action::Jump] = { 'J', XINPUT_GAMEPAD_A};
    bindings[(int)Action::Attack] = { 'K', XINPUT_GAMEPAD_X };
    bindings[(int)Action::Guard] = { 'I', XINPUT_GAMEPAD_Y};
    bindings[(int)Action::Dodge] = { 'L', XINPUT_GAMEPAD_B };
    bindings[(int)Action::Debug] = { 'O', XINPUT_GAMEPAD_BACK };
}

void ActionInput::Update()
{
    keyboard.Update();
    gamepad.Update();

    for (int i = 0; i < (int)Action::ActionMax; ++i) {
        const ActionBinding& b = bindings[i];

        bool down =
            keyboard.Press(b.key) ||
            gamepad.Press(b.btn);

        states[i].Update(down);
    }
}

bool ActionInput::IsPress(Action a) const
{
    return states[(int)a].Press();
}

bool ActionInput::IsTrigger(Action a) const
{
    return states[(int)a].Trigger();
}

bool ActionInput::IsRelease(Action a) const
{
    return states[(int)a].Release();
}

float ActionInput::GetMoveDirectionRad() const
{
    bool w = IsPress(Action::MoveUp);
    bool s = IsPress(Action::MoveDown);
    bool a = IsPress(Action::MoveLeft);
    bool d = IsPress(Action::MoveRight);

    if (w && a) return 3.0f * PI / 4.0f;
    if (w && d) return 5.0f * PI / 4.0f;
    if (s && a) return 1.0f * PI / 4.0f;
    if (s && d) return 7.0f * PI / 4.0f;
    if (w)      return PI;
    if (s)      return 0.0f;
    if (a)      return PI / 2.0f;
    if (d)      return 3.0f * PI / 2.0f;

    return -1.0f;
}