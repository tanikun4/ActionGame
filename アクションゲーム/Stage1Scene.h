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
	enum class WaveState
	{
		None,           // 通常進行
		WaveEffect,     // Wave表示中
		SpawnEnemy,     // 敵生成待ち
		BossBattle        // ボス戦闘
	};

	Sound sound;
	std::vector<Object*> m_MySceneObjects; // このシーンのオブジェクト

	DirectX::XMFLOAT2 groundsize;//地面の大きさ

	Player* player;
	Boss* boss;
	Ground* ground;

	WaveState m_waveState = WaveState::None; //現在のWave状態
	int wave = 0; //現在のWave数　
	const int maxwave = 3;//最大Wave数。このWaveに到達するとボスが出現し、倒せばクリア。

	int framecount = 0;//フレームカウント用
	Texture2D* boss_hp_text = nullptr;// ボスHP文字
	Texture2D* boss_guard_text = nullptr;// ボスガードゲージ文字
	Texture2D* wave1_text = nullptr;// Wave1文字
	Texture2D* wave2_text = nullptr;// Wave2文字
	Texture2D* finalwave_text = nullptr;// FinalWave文字
	Texture2D* sousa_key_text = nullptr;// 操作説明、キー表示
	Texture2D* sousa_cont_text = nullptr;// 操作説明文字、コントローラー表示

	void Init(); // 初期化
	void Uninit(); // 終了処理

	void WaveChange(); // Wave変更処理
	void UpdateWaveCheck(); // Wave変更チェック
	void UpdateWaveEffect(); // Waveエフェクト更新
	void UpdateSpawnEnemy(); // 敵生成更新
	void UpdateBossWait(); // ボス待機更新

	
public:
	Stage1Scene(); // コンストラクタ
	~Stage1Scene(); // デストラクタ

	void Update(); // 更新
};

