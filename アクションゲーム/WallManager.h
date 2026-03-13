#pragma once
#include "Manager.h"
#include "Cube.h"
#include <vector>
#include <SimpleMath.h>

// 壁管理クラス、壁のセットとデバッグ用UIを担当
class WallManager :
	public Manager<WallManager>
{
	friend class Manager<WallManager>; // Manager から new できるようにする

public:
	void SetWall(const DirectX::SimpleMath::Vector2& groundsize,std::vector<Object*>& _scene_object);
	std::vector<Cube*> GetWalls() { return walls; };//壁オブジェクトのポインタを保存するベクターを取得する関数
	void ClearPointer() { walls.clear(); };//壁オブジェクトの解放
	void DebugWallStatus();//壁の大きさや位置を操作する、外部呼出しで登録できる
private:
	std::vector<Cube*> walls;//壁オブジェクトのポインタを保存するベクター
};

