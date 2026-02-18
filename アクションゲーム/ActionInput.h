#pragma once
#include "Action.h"
#include "ActionState.h"
#include "ActionBinding.h"
#include "KeyboardState.h"
#include "GamePadState.h"
#include "MathCommon.h"

class ActionInput {
public:
    static ActionInput& GetInstance();

    void Init();
    void Update();
    void Uninit();

    bool IsPress(Action a)   const;
    bool IsTrigger(Action a) const;
    bool IsRelease(Action a) const;

    DirectX::SimpleMath::Vector2 GetMoveVector() const;

    float GetMoveDirectionRad() const;

    DirectX::SimpleMath::Vector2 GetCameraVector() const;

	bool GetControllerInput() const { return cont_Input; } // コントローラー入力があるかどうか

private:
    ActionInput() = default;                  // 外部生成禁止
    ~ActionInput() = default;
    ActionInput(const ActionInput&) = delete;
    ActionInput& operator=(const ActionInput&) = delete;
private:
    KeyboardState keyboard;
    GamePadState  gamepad;

    ActionState   states[(int)Action::ActionMax];
    ActionBinding bindings[(int)Action::ActionMax] = {};

	bool cont_Input = false; // コントローラー入力フラグ
};
