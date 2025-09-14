#pragma once
#include <memory>


class Texture2D;
class Camera;

class Fade //フェード処理を管理するクラス、シングルトン
{
private:
	static std::unique_ptr<Fade> m_Instance; // ゲームインスタンス
    int  flamecount = 0;
	const int maxflame = 30;
	std::unique_ptr<Texture2D> fadetex;
	bool fadeout = false;
	bool fadein = false;
	const float  alpha_val = 0.01f;//1フレームあたりのアルファ値変化量

	bool FadeOut();//フェードアウト処理
	bool FadeIn();//フェードイン処理
public:
	void Init(Camera* cam);
	void Update();
	void Draw();
	void Uninit();
	void SetCamera(Camera* cam);
	void StartFadeOut() { fadeout = true; flamecount = 0; };//フェードアウト開始
	void StartFadeIn() { fadein = true; flamecount = 0; };//フェードイン開始

	bool FinishedFadeOut() { return !fadeout; };//フェードアウトが終了したか
	bool FinishedFadeIn() { return !fadein; };// フェードインが終了したか
	static Fade* GetInstance();
};

