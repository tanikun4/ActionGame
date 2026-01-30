#pragma once
#include "Scene.h"
#include "Object.h"
#include "sound.h"

class Player;
class Enemy;
class Boss;
class Ground;
class Texture2D;

// Stage1Scenクラス
class Stage1Scene : public Scene
{
private:
	Sound sound;
	std::vector<Object*> m_MySceneObjects; // このシーンのオブジェクト
	//std::vector<Texture2D*> m_UI; // UI用オブジェクト
	int m_State = 0; // 状態 0:ボール移動中, 1:方向選択中, 2:パワー選択中

	DirectX::XMFLOAT2 groundsize;//地面の大きさ

	Player* player;
	Boss* boss;
	Ground* ground;

	int wave = 0; //現在のWave数　
	const int maxwave = 3;//最大Wave数。このWaveに到達するとボスが出現し、倒せばクリア。

	int framecount = 0;//フレームカウント用
	Texture2D* boss_hp_text = nullptr;// ボスHP文字

	void Init(); // 初期化
	void Uninit(); // 終了処理
public:
	Stage1Scene(); // コンストラクタ
	~Stage1Scene(); // デストラクタ

	void Update(); // 更新
};

