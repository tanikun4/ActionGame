#pragma once
#include "Manager.h"
#include <vector>
#include <SimpleMath.h>

class Object;
// 壁管理クラス、壁のセットとデバッグ用UIを担当
class WallManager :
	public Manager<WallManager>
{
	friend class Manager<WallManager>; // Manager から new できるようにする

public:
	static void SetWall(const DirectX::SimpleMath::Vector2& groundsize,std::vector<Object*>& _scene_object);
	static void DebugWallStatus();//壁の大きさや位置を操作する、外部呼出しで登録できる
private:
};

