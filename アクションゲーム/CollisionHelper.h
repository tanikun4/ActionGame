#include "ICollider.h" // ここで両方取り込む（ヘッダ間の循環はないためOK）
#include <variant>

namespace Collision
{
    inline bool CheckHit(ICollider& a, ICollider& b)
    {
        CollisionResult result;

        bool hit = std::visit([&](auto&& lhs, auto&& rhs) -> bool {
            return Collision::CheckHit(lhs, rhs, result);
            }, a.GetCollision(), b.GetCollision());

        if (hit) {
            a.SetCollisionResult(result);
            b.SetCollisionResult(result);
        }
        return hit;
    }
}