#pragma once
#include <iostream>

#include "Camera.h"
#include "TitleScene.h"
#include "Stage1Scene.h"
#include "ResultScene.h"
#include "GameOverScene.h"
#include "WireRenderer.h"

enum SceneName {
	TITLE,
	STAGE1,
	RESULT,
	GAMEOVER,
};

class Game
{

private:

	static Game* m_Instance; // ゲームインスタンス

	Scene* m_Scene = nullptr; // シーン

	std::vector<std::unique_ptr<Object>> m_Objects; // オブジェクト
	//std::unique_ptr<Input> m_Input;  // 入力処理
	std::unique_ptr<Camera> m_Camera; // カメラ
	std::unique_ptr<WireRenderer> m_WireRenderer; // ワイヤーレンダラー
	int max_stop_frame = 3;//ヒットストップのフレーム
	int stop_frame = 0;
	bool stop = false;// trueの間はオブジェクトのUpdateを止める
	bool slow = false; // スローモーション中かどうか
	int slow_frame = 0; // スローモーションのフレーム管理
	int max_slow_frame = 30; // スローモーションの最大フレーム数
	bool change_request = false; // シーン変更要求
	SceneName m_NextScene = TITLE;

	bool debugmode = false; // デバッグモード
public:
	Game(); // コンストラクタ
	~Game(); // デストラクタ

	static void Init(); // 初期化
	static void Update(); // 更新
	static void Draw(); // 描画
	static void Uninit(); // 終了処理
	
	static Game* GetInstance();

	void HitStop(int _maxstop = 3);// ヒットストップ開始
	void SlowMotion(int _maxslow = 120); // スローモーション開始
	void ChangeScene(SceneName sName); // シーンを変更
	void ChangeSceneFadeOut(SceneName sName); // フェードアウトしてシーンを変更
	Camera& GetCamera(); // カメラ取得
	void DeleteObject(Object* pt); // オブジェクトを削除する
	void DeleteAllObject(); // オブジェクトをすべて削除する

	void CollisionObject(std::vector<Object*>& Object);// 渡されたオブジェクト全ての衝突判定を行う
	

	// オブジェクトを追加する(※テンプレート関数なのでここに直接記述)
	//template<class T> T* AddObject(DrawLayer layer = DrawLayer::World3D)
	//{
	//	auto obj = std::make_unique<T>(m_Camera.get());

	//	T* pt = new T(m_Camera.get());
	//	pt->SetLayer(layer);
	//	m_Instance->m_Objects.emplace_back(pt);
	//	pt->Init(); // 初期化
	//	return pt;
	//}
	template<class T>
	T* AddObject(DrawLayer layer = DrawLayer::World3D)
	{
		auto obj = std::make_unique<T>(m_Camera.get());

		obj->SetLayer(layer);

		T* ptr = obj.get();

		m_Instance->m_Objects.emplace_back(std::move(obj));

		ptr->Init();

		return ptr;
	}

	// オブジェクトを取得する(※テンプレート関数なのでここに直接記述)
	template<class T> std::vector<T*> GetObjects()
	{
		std::vector<T*> res;
		for (auto& o : m_Instance->m_Objects) {
			// dynamic_castで型をチェック
			if (T* derivedObj = dynamic_cast<T*>(o.get())) {
				res.emplace_back(derivedObj);
			}
		}
		return res;
	}

};
