//=============================================================================
//
// DirectXのチームロゴのソースファイル [teamLogo.cpp]
// Author : TENMA SAITO
//
//=============================================================================
//*****************************************************************************
//*** インクルードファイル ***
//*****************************************************************************
#include "teamLogo.h"
#include "fade.h"
#include "Texture.h"
#include "2Dpolygon.h"
#include "param.h"
#include "input.h"
#include "sound.h"

//*****************************************************************************
//*** マクロ、定数宣言 ***
//*****************************************************************************
constexpr int MAX_COUNTWAIT = 360;	// フェードするまでの猶予時間

//*****************************************************************************
//*** グローバル変数 ***
//*****************************************************************************
int g_nCounterTeamLogo;		// チームロゴ表示中のカウンター

//=============================================================================
// --- 初期化 ---
//=============================================================================
void InitTeamLogo(void)
{
	// 名前空間省略
	using namespace Constants;

	/*** テクスチャの初期化 ***/
	InitTexture();

	/*** 2Dポリゴンの初期化 ***/
	Init2DPolygon();

	// テクスチャの読み込み
	int nIdxTexture;
	LoadTexture("data/TEXTURE/FKeyProject.png", &nIdxTexture);

	// ポリゴンの設置
	Set2DPolygon(CParamVector::WINMID, CParamVector::V3NULL, D3DXVECTOR2(300.0f, 300.0f), nIdxTexture);

	g_nCounterTeamLogo = 0;
}

//=============================================================================
// --- 終了 ---
//=============================================================================
void UninitTeamLogo(void)
{
	/*** テクスチャの終了 ***/
	UninitTexture();

	/*** 2Dポリゴンの終了 ***/
	Uninit2DPolygon();
}

//=============================================================================
// --- 更新 ---
//=============================================================================
void UpdateTeamLogo(void)
{
	/*** 2Dポリゴンの更新 ***/
	Update2DPolygon();

	if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(0, JOYKEY_START)
		|| GetJoypadTrigger(1, JOYKEY_A)
		|| GetJoypadTrigger(1, JOYKEY_START))
	{
		if (GetFade() == FADE_NONE)
		{
			SetFade(MODE_TITLE);
			FadeSound(SOUND_LABEL_BGM_TITLE);
		}
	}

	if (g_nCounterTeamLogo >= MAX_COUNTWAIT)
	{
		if (GetFade() == FADE_NONE)
		{
			SetFade(MODE_TITLE);
			FadeSound(SOUND_LABEL_BGM_TITLE);
		}
	}
	else
	{
		// カウンターの増加
		g_nCounterTeamLogo++;
	}
}

//=============================================================================
// --- 描画 ---
//=============================================================================
void DrawTeamLogo(void)
{
	/*** 2Dポリゴンの描画 ***/
	Draw2DPolygon();
}