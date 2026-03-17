//==================================================================================
//
// DirectXのリザルト関連用のcppファイル [Result.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "Result.h"
#include "input.h"
#include "fade.h"
#include "badend.h"
#include "normalend.h"
#include "happyend.h"
#include "sound.h"
#include "2Dpolygon.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
ENDTYPE g_endType = ENDTYPE_NORMAL;		// エンディングタイプ
int g_nCounterEnding = 0;				// エンディングカウンター

//**********************************************************************************
//*** 定数変数 ***
//**********************************************************************************
constexpr int c_EndingTime = 240;		// エンディング持続時間

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitResult(void)
{
	/*** エンディングの初期化 ***/
	switch (g_endType)
	{
	// バッドエンド
	case ENDTYPE_BAD:
		FadeSound(SOUND_LABEL_BGM_BADEND);
		InitBadEnd();
		break;

	// ノーマルエンド
	case ENDTYPE_NORMAL:
		FadeSound(SOUND_LABEL_BGM_NORMALEND);
		InitNormalEnd();
		break;

	// ハッピーエンド
	case ENDTYPE_HAPPY:
		FadeSound(SOUND_LABEL_BGM_SUPERHAPPYEND);
		InitHappyEnd();
		break;

	// 例外エンド
	default:
		break;
	}

	g_nCounterEnding = 0;
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitResult(void)
{
	// 2Dポリゴンの終了処理
	Uninit2DPolygon();
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateResult(void)
{
	// モード変更
	if (GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(0, JOYKEY_A)
		|| GetJoypadTrigger(0, JOYKEY_START)
		|| g_nCounterEnding >= c_EndingTime)
	{
		if (GetFade() == FADE_NONE)
		{
			SetFade(MODE_TEAMLOGO);
			StopSound();
		}
	}

	/*** エンディングの更新 ***/
	switch (g_endType)
	{
		// バッドエンド
	case ENDTYPE_BAD:
		UpdateBadEnd();
		Update2DPolygon();
		break;

		// ノーマルエンド
	case ENDTYPE_NORMAL:
		UpdateNormalEnd();
		Update2DPolygon();
		break;

		// ハッピーエンド
	case ENDTYPE_HAPPY:
		UpdateHappyEnd();
		Update2DPolygon();
		break;

		// 例外エンド
	default:
		break;
	}

	g_nCounterEnding++;
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawResult(void)
{
	/*** エンディングの描画 ***/
	switch (g_endType)
	{
		// バッドエンド
	case ENDTYPE_BAD:
		DrawBadEnd();
		Draw2DPolygon();
		break;

		// ノーマルエンド
	case ENDTYPE_NORMAL:
		DrawNormalEnd();
		Draw2DPolygon();
		break;

		// ハッピーエンド
	case ENDTYPE_HAPPY:
		DrawHappyEnd();
		Draw2DPolygon();
		break;

		// 例外エンド
	default:
		break;
	}
}

//==================================================================================
// --- エンディングタイプの指定 ---
//==================================================================================
void SetEndingType(ENDTYPE end)
{
	g_endType = end;
}