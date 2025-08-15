#pragma once
class IColliderBase {
public:
    virtual ~IColliderBase() = default;
    virtual void GetCollision() = 0; // ‰½‚ç‚©‚Ì‹¤’ÊŠî’êŒ^
};