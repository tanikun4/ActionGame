#pragma once
#include "Scene.h"
#include "Object.h"
#include "sound.h"

class Player;
class Enemy;
class Boss;
class Ground;

// Stage1Scenクラス
class Stage1Scene : public Scene
{
private:
	Sound sound;
	std::vector<Object*> m_MySceneObjects; // このシーンのオブジェクト
	int m_State = 0; // 状態 0:ボール移動中, 1:方向選択中, 2:パワー選択中

	void Collision();
	void Init(); // 初期化
	void Uninit(); // 終了処理

	int m_Par; // パー(標準打数)
	int m_StrokeCount; // 現在の打数
	const int setenemycount = 15;//敵の数
	DirectX::XMFLOAT2 groundsize;//地面の大きさ

	Player* player;
	Boss* boss;
	Ground* ground;

	void SetWall();
	void DebugWallStatus();

public:
	Stage1Scene(); // コンストラクタ
	~Stage1Scene(); // デストラクタ

	void Update(); // 更新

	int GetScore(); // スコアを取得
};

