#pragma once
#include "MathCommon.h"
#include "Texture2D.h"
#include <algorithm>
#include "Game.h"
struct HpGauge
{
	Texture2D* gaugeTex;
	Texture2D* backTex;
	float baseScale_X;

    void Init(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scale) {
        backTex = Game::GetInstance()->AddObject<Texture2D>();
        backTex->SetTexture("assets/texture/white.png"); // 画像を指定
        backTex->SetPosition(pos);
        backTex->SetScale(scale);
        backTex->SetColor({ 0,0,0,1 });
        backTex->SetUV(1, 1, 1, 1); //UVを指定
        baseScale_X = scale.x;

        gaugeTex = Game::GetInstance()->AddObject<Texture2D>();
        gaugeTex->SetTexture("assets/texture/white.png"); // 画像を指定
        gaugeTex->SetPosition(pos);
        gaugeTex->SetScale(scale);
        gaugeTex->SetColor({ 0,1,0,1 });
        gaugeTex->SetUV(1, 1, 1, 1); //UVを指定

    }

    void ChangeGauge(int cuHp, int maxHp)
    {
        cuHp = std::clamp(cuHp, 0, maxHp);
		float rate = (float)cuHp / (float)maxHp;

		DirectX::SimpleMath::Vector3 gaugeScale = gaugeTex->GetScale();

        float currentWidth = baseScale_X * rate;

        gaugeTex->SetScale(currentWidth,gaugeScale.y,gaugeScale.z);

        // 左固定補正
        float offsetX = (gaugeScale.x - currentWidth) * 0.5f;

        DirectX::SimpleMath::Vector3 gaugePos = gaugeTex->GetPosition();
		gaugePos.x -= offsetX;
		gaugeTex->SetPosition(gaugePos);
	}

    void DrawHpGauge()
    {
        backTex->Draw();
        gaugeTex->Draw();
    }

    void SetColor(DirectX::SimpleMath::Vector4 color) {
        gaugeTex->SetColor(color);
	}

    void SetPosScale(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scale) {
        gaugeTex->SetPosition(pos);
        gaugeTex->SetScale(scale);
        backTex->SetPosition(pos);
        backTex->SetScale(scale);
    }

    void Uninit() {
		if (!gaugeTex || !backTex) return;
        gaugeTex->Uninit();
        backTex->Uninit();
    }

    Texture2D* GetGauge(int no) {
        if (no == 0) {
            return backTex;
        }
        return gaugeTex;
    }
};