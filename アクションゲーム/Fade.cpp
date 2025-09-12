#include "Fade.h"
#include "Texture2D.h"
#include "Camera.h"

std::unique_ptr<Fade> Fade::m_Instance = nullptr;


void Fade::Init(Camera* cam) 
{
	m_Instance = std::make_unique<Fade>();
	fadetex = std::make_unique<Texture2D>(cam);
	fadetex->Init();
	fadetex->SetTexture("assets/texture/fade.png");
	fadetex->SetPosition(0.0f, 0.0f, 0.0f);
	fadetex->SetRotation(0.0f, 0.0f, 0.0f);
	fadetex->SetScale(1280.0f, 720.0f, 0.0f);
	fadetex->SetColor({ 0,0,0,0 });
	fadein = false;
	fadeout = false;
	flamecount = 0;
}

void Fade::Update() 
{
	if (fadein) { fadein = FadeIn(); };
	if (fadeout) { fadeout = FadeOut(); };
}

void Fade::Draw() 
{
	fadetex->Draw();
}

void Fade::Uninit() 
{
	
}

Fade* Fade::GetInstance() {
	//if (m_Instance == nullptr) {
	//	m_Instance = std::make_unique<Fade>();
	//}
	return m_Instance.get();
}

bool Fade::FadeOut() 
{
	fadetex->SetColor({ 0,0,0,fadetex->GetAlpha() + alpha_val });
	if(fadetex->GetAlpha() >= 1.0f){
		return false;
	}
	else {
		return true;
	}
}

bool Fade::FadeIn() 
{
	fadetex->SetColor({ 0,0,0,fadetex->GetAlpha() - alpha_val});
	if (fadetex->GetAlpha() <= 0.0f) {
		return false;
	}
	else {
		return true;
	}
}
