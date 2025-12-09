#pragma once
#include "Object.h"
#include "EffectDataStruct.h"

//エフェクト基底クラス
class EffectBase : public Object
{
protected:
	int m_lifeframe = 0;//エフェクトの現在生存フレーム
	int m_maxlife = 0;  //エフェクトの最大生存フレーム
	int m_poschange_frame = 0;//座標変更開始フレーム数
	int m_rotchange_frame = 0;//回転開始更フレーム数
	int m_scalechange_frame = 0;//スケール変更開始フレーム数

	bool m_scalechange = false;//スケール変更するかどうかのフラグ

	DirectX::SimpleMath::Vector3 pos_changerate = { 0, 0, 0 };//座標変化量
	DirectX::SimpleMath::Vector3 rot_changerate = { 0, 0, 0 };//回転変化量
	DirectX::SimpleMath::Vector3 scale_changerate = { 0, 0, 0};//スケール変化量
public:
	EffectBase();
	EffectBase(Camera* cam);
	~EffectBase();
    virtual void Update() override = 0;
    virtual void Draw() override = 0;

	//エフェクト用の初期化
	virtual void Init(LoadedEffectData& _data, int _maxlife, 
		DirectX::SimpleMath::Vector3 _pos_amount,int _pos_changeframe, 
		DirectX::SimpleMath::Vector3 _rot_amount, int _rot_changeframe,
		DirectX::SimpleMath::Vector3 _scale_amount, int _scale_changeframe) { };//オーバーライド用ダミー

	void BaseInit(int _maxlife, 
		DirectX::SimpleMath::Vector3 _pos_amount, int _pos_changeframe,
		DirectX::SimpleMath::Vector3 _rot_amount, int _rot_changeframe,
		DirectX::SimpleMath::Vector3 _scale_amount, int _scale_changeframe);//共通初期化処理

	void BaseUpdate();//共通更新処理

	void ResetLife() { m_lifeframe = 0; m_live = true; }
	void SetLifeFlame(float flame) { m_lifeframe = flame; }


	/*void PlayEffect(
		int _id, DirectX::SimpleMath::Vector3 _pos,
		DirectX::SimpleMath::Vector3 _rot,
		DirectX::SimpleMath::Vector3 _scale,
		int _maxlife = 60,
		DirectX::SimpleMath::Vector3 _scale_amount = { -1, -1, -1 });*/
};

