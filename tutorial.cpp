//==================================================================================
//
// DirectXのチュートリアルのソースファイル [tutorial.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "tutorial.h"

// 基本インクルード群
#include "main.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

// 汎用インクルード群
#include "Texture.h"
#include "2Dpolygon.h"
#include "mathUtil.h"
#include "MessageLog.h"
#include "param.h"

// 通常インクルード群
#include "dialog.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define LOG_POS		D3DXVECTOR3(100.0f, 450.0f, 0.0f)	// ログの位置
#define LOG_WIDTH	(1000.0f)	// ログの幅
#define LOG_HEIGHT	(250.0f)	// ログの高さ

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
LPMESSAGELOG g_pLog = NULL;		// メッセージログへのポインタ

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitTutorial(void)
{
	AUTODEVICE9 Auto;

	// テクスチャの初期化
	InitTexture();

	// 2Dポリゴンの初期化
	Init2DPolygon();

	// メッセージログの初期化
	InitMessageLog(&g_pLog, sizeof(g_pLog));

	// ダイアログの初期化
	InitDialog();

	// ダイアログの設置
	SetDialog();

	USE_LIB;
	USE_PARAM;
	WString testString = L"あーあー、テストテスト。これ見えてる？";

	// メッセージログの作成
	CreateMessageLogW(Auto.pDevice,
		LOG_POS,
		LOG_WIDTH,
		LOG_HEIGHT,
		DEF_FONTSIZE,
		testString.GetHead(),
		testString.GetVectorNum(),
		5,
		CParamColor::WHITE,
		&g_pLog);
}

//==================================================================================
// --- 終了 ---
//==================================================================================
void UninitTutorial(void)
{
	// テクスチャの破棄
	UninitTexture();

	// 2Dポリゴンの破棄
	Uninit2DPolygon();

	// メッセージログの破棄
	UninitMessageLog(&g_pLog, sizeof(g_pLog));
}

//==================================================================================
// --- 更新 ---
//==================================================================================
void UpdateTutorial(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		if (GetFade() == FADE_NONE)
		{
			USE_LIB;

			WString testString = L"えっ、ちょっと待ってよ未だ話したいこといっぱいあるのに";

			g_pLog->ReplaceStringW(testString.GetHead(),
				testString.GetVectorNum(),
				KEEP_USING,
				KEEP_USING_COL,
				TRUE);

			SetFade(MODE_GAME);
			FadeSound(SOUND_LABEL_BGM_GAME);
		}
	}

	// 2Dポリゴンの更新
	Update2DPolygon();

	// メッセージログの更新
	UpdateMessageLog(&g_pLog, sizeof(g_pLog));
}

//==================================================================================
// --- 描画 ---
//==================================================================================
void DrawTutorial(void)
{
	// 2Dポリゴンの描画
	Draw2DPolygon();

	// メッセージログの描画
	DrawMessageLog(&g_pLog, sizeof(g_pLog));
}