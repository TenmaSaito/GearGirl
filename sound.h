//=============================================================================
//
// サウンド処理 [sound.h]
// Author : TENMA SAITO (SPACIAL THANKS : AKIRA TANAKA)
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

// マクロ定義
#define SOUND_BGMSTART		(SOUND_LABEL_BGM_TITLE)		// BGMの開始位置
#define SOUND_SESTART		(SOUND_LABEL_SE_WALK)		// SEの開始位置
#define SOUND_STANDARD		(0.8f)						// 初期音量

#ifndef NDEBUG
#undef SOUND_STANDARD
#define SOUND_STANDARD		(0.0f)						// 初期音量
#endif

#define _3DAUDIO
#define SOUND_SPD			D3DXVECTOR3(1, 1, 1)		// 音速度

//*****************************************************************************
// サウンド一覧 (sound.cppのsoundinfoにも追加する！)
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM_TITLE = 0,		// タイトル画面のBGM
	SOUND_LABEL_SE_WALK,			// 歩行音
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// サウンドの種類
//*****************************************************************************
typedef enum
{
	SETSOUND_GAME = 0,			// ゲーム内音声
	SETSOUND_MENU_SE,			// システム音声
	SETSOUND_MAX
}SETSOUND;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);

// 追加プロトタイプ宣言
bool GetSound(SOUND_LABEL label);
void SetVolume(float volume, SETSOUND sound);
float GetVolume(SETSOUND sound);
void FadeSound(SOUND_LABEL label);
SOUND_LABEL GetPlaySound(void);
void UpdateSound(void);
void Play3DSound(SOUND_LABEL label, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 posU);

#endif
