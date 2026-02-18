#include "ActionInput.h"

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
    bindings[(int)Action::Jump] = { 'L', XINPUT_GAMEPAD_A};
    bindings[(int)Action::Attack] = { 'K', XINPUT_GAMEPAD_X };
    bindings[(int)Action::Guard] = { 'I', XINPUT_GAMEPAD_Y};
    bindings[(int)Action::Dodge] = { 'J', XINPUT_GAMEPAD_B };
    bindings[(int)Action::Debug] = { 'O', XINPUT_GAMEPAD_BACK };
	bindings[(int)Action::Enter] = { VK_RETURN, XINPUT_GAMEPAD_START };
	// ここのコントローラ割り当ては変える、右スティックが良いかも
    bindings[(int)Action::Up] = { VK_UP, XINPUT_GAMEPAD_LEFT_THUMB };
    bindings[(int)Action::Down] = { VK_DOWN, XINPUT_GAMEPAD_RIGHT_THUMB };
    bindings[(int)Action::Left] = { VK_LEFT, XINPUT_GAMEPAD_LEFT_SHOULDER };
    bindings[(int)Action::Right] = { VK_RIGHT, XINPUT_GAMEPAD_RIGHT_SHOULDER };
}

void ActionInput::Update()
{
    keyboard.Update();
    gamepad.Update();

    for (int i = 0; i < (int)Action::ActionMax; ++i) {
        const ActionBinding& b = bindings[i];

        bool key = keyboard.Press(b.key);
        bool pad = gamepad.Press(b.btn);

		cont_Input = pad;

		bool down = key || pad;

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

DirectX::SimpleMath::Vector2 ActionInput::GetMoveVector() const
{
    // キーボード、コントローラ十字キー
    float x = 0;
    float y = 0;

    if (IsPress(Action::MoveRight)) x -= 1;
    if (IsPress(Action::MoveLeft)) x += 1;
    if (IsPress(Action::MoveUp)) y -= 1;
    if (IsPress(Action::MoveDown)) y += 1;

    DirectX::SimpleMath::Vector2 keyVec = { x, y };
    if (keyVec.Length() > 1) keyVec.Normalize();

    // スティック
    DirectX::SimpleMath::Vector2 stick = gamepad.GetLeftStick();

    // スティック優先
    if (stick.Length() > 0.01f)
        return stick;

    return keyVec;
}

float ActionInput::GetMoveDirectionRad() const
{

    DirectX::SimpleMath::Vector2 v = GetMoveVector();

    if (v.Length() < 0.01f)
        return -1.0f;

    // atan2(y, x) ではなく
    // 今の座標系に合わせる
    return atan2f(v.x, v.y);
}

DirectX::SimpleMath::Vector2 ActionInput::GetCameraVector() const
{
    // キーボード
    float x = 0;
    float y = 0;

    if (IsPress(Action::Right)) x -= 1;
    if (IsPress(Action::Left)) x += 1;
    if (IsPress(Action::Up)) y -= 1;
    if (IsPress(Action::Down)) y += 1;

    DirectX::SimpleMath::Vector2 keyVec = { x, y };
    if (keyVec.Length() > 1) keyVec.Normalize();

    // スティック
    DirectX::SimpleMath::Vector2 stick = gamepad.GetRightStick();

    // スティック優先
    if (stick.Length() > 0.01f)
        return stick;

    return keyVec;
}