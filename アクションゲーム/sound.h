#pragma once

#include <xaudio2.h>

// サウンドファイル
typedef enum
{
	SOUND_BGM_MAIN = 0,		
	SOUND_BGM_TITLE,

	SOUND_SE_SWING,			
	SOUND_SE_SWORDHIT,			
	SOUND_SE_ARROWCHARGE,				
	SOUND_SE_ARROWSHOT,				
	SOUND_SE_ARROWHIT,
	SOUND_SE_PLAYERHIT,
	SOUND_SE_PLAYERGUARD,
	SOUND_SE_PLAYERJUSTGUARD,
	SOUND_SE_PLAYERSHOOTBACK,
	SOUND_SE_SWINGVERTICAL,
	SOUND_SE_ROTATEATTACK,

	SOUND_LABEL_MAX,
} SOUND_LABEL;

class Sound {
private:
	static Sound* m_Instance; // ゲームインスタンス
	// パラメータ構造体
	typedef struct
	{
		LPCSTR filename;	// 音声ファイルまでのパスを設定
		bool bLoop;			// trueでループ。通常BGMはture、SEはfalse。
	} PARAM;

	PARAM m_param[SOUND_LABEL_MAX] =
	{
		{"assets/Sound/BGM/maou_bgm_fantasy15.wav", true},	
		{"assets/Sound/BGM/sentoukei_02.wav", true},
		{"assets/Sound/SE/swing.wav", false},
		{"assets/Sound/SE/swordhit.wav", false},		
		{"assets/Sound/SE/arrowcharge.wav", false},				
		{"assets/Sound/SE/arrowshot.wav", false},
		{"assets/Sound/SE/arrowhit.wav", false},
		{"assets/Sound/SE/hit.wav", false},		
		{"assets/Sound/SE/guard.wav", false},
		{"assets/Sound/SE/justguard.wav", false},
		{"assets/Sound/SE/shootback.wav", false},
		{"assets/Sound/SE/swing_vertical.wav", false},
		{"assets/Sound/SE/rotateattack.wav", false},

	};

	IXAudio2* m_pXAudio2 = NULL;
	IXAudio2MasteringVoice* m_pMasteringVoice = NULL;
	IXAudio2SourceVoice* m_pSourceVoice[SOUND_LABEL_MAX];
	WAVEFORMATEXTENSIBLE m_wfx[SOUND_LABEL_MAX]; // WAVフォーマット
	XAUDIO2_BUFFER m_buffer[SOUND_LABEL_MAX];
	BYTE* m_DataBuffer[SOUND_LABEL_MAX];

	HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
	HRESULT ReadChunkData(HANDLE, void*, DWORD, DWORD);

public:
	static void SoundInit();

	// ゲームループ開始前に呼び出すサウンドの初期化処理
	HRESULT Init(void);

	// ゲームループ終了後に呼び出すサウンドの解放処理
	void Uninit(void);

	// 引数で指定したサウンドを再生する
	void Play(SOUND_LABEL label);

	// 引数で指定したサウンドを停止する
	void Stop(SOUND_LABEL label);

	// 引数で指定したサウンドの再生を再開する
	void Resume(SOUND_LABEL label);

	// 引数で指定したサウンドの音量を変更する
	void SetVolume(SOUND_LABEL label, float volume);

	// 全体のマスターボリュームを変更する
	void SetMasterVolume(float volume);
	
	static Sound* GetInstance();

};