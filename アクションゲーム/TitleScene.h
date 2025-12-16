#pragma once
#include "Scene.h"
#include "Object.h"
#include "Texture2D.h"

// TitleSceneクラス
class TitleScene : public Scene
{
private:
	std::vector<Object*> m_MySceneObjects; // このシーンのオブジェクト
	Texture2D* press_enterkey;
	void Init(); // 初期化
	void Uninit(); // 終了処理

	int count = 0;
	bool set = false;

public:
	TitleScene(); // コンストラクタ
	~TitleScene(); // デストラクタ

	void Update(); // 更新
};

